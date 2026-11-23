// UserMenu.cpp
#include "UserMenu.h"
#include "PlateValidator.h"
#include <iostream>
#include <string>

namespace {

const int MENU_EOF     = -1;
const int MENU_INVALID = -999;

int readInt() {
    std::string line;
    if (!std::getline(std::cin, line)) return MENU_EOF;
    try {
        std::size_t pos = 0;
        int v = std::stoi(line, &pos);
        if (pos != line.size()) return MENU_INVALID;
        return v;
    } catch (...) {
        return MENU_INVALID;
    }
}

std::string readLine() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

//Reading an hour from 0-23, illegally returning -1
int readHour(const std::string& prompt) {
    std::cout << prompt;
    int h = readInt();
    if (h < 0 || h > 23) {
        std::cout << "Invalid hour. Please enter an integer between 0 and 23.\n";
        return -1;
    }
    return h;
}

void printUserMenu() {
    std::cout << "\n============ User Menu ============\n";
    std::cout << "1. Vehicle entry\n";
    std::cout << "2. Vehicle exit\n";
    std::cout << "3. Cancel queue\n";
    std::cout << "0. Return to main menu\n";
    std::cout << "-----------------------------------\n";
    std::cout << "Choice: ";
}

void handleEnter(ParkingLot& lot) {
    std::cout << "Enter plate number: ";
    std::string plate = readLine();

    if (!isValidHKPlate(plate)) {
        std::cout << "Invalid HK plate format. "
                     "Expected 2-3 uppercase letters followed by 1-4 digits.\n";
        return;
    }

    int hour = readHour("Enter entry hour (0-23): ");
    if (hour < 0) return;

    EnterResult r = lot.enterVehicle(plate, hour);

    switch (r.status) {
        case EnterResult::SUCCESS:
            std::cout << "Welcome, plate " << r.plate
                      << ", entry hour " << r.hour
                      << ", remaining spaces " << r.remainingSpaces << ".\n";
            break;

        case EnterResult::ALREADY_PARKED:
            std::cout << "Vehicle has already entered. "
                         "Please do not repeat the operation.\n";
            break;

        case EnterResult::ALREADY_QUEUED:
            std::cout << "Vehicle is already in the waiting queue. "
                         "Please do not repeat the operation.\n";
            break;

        case EnterResult::QUEUED:
            std::cout << "Sorry, the parking lot is full. "
                      << "Now joining the queue. "
                      << "Current number of queued vehicles: "
                      << r.queueSize << ".\n";
            break;
    }
}

void handleExit(ParkingLot& lot) {
    std::cout << "Enter plate number: ";
    std::string plate = readLine();

    if (!isValidHKPlate(plate)) {
        std::cout << "Invalid HK plate format. "
                     "Expected 2-3 uppercase letters followed by 1-4 digits.\n";
        return;
    }

    int hour = readHour("Enter exit hour (0-23): ");
    if (hour < 0) return;

    ExitResult r = lot.exitVehicle(plate, hour);

    switch (r.status) {
        case ExitResult::SUCCESS:
            std::cout << "Exit successful. "
                      << "Parking duration: " << r.durationHours << " hour(s). "
                      << "Fee: " << r.fee << " HKD. "
                      << "Remaining spaces: " << r.remainingSpaces << ".\n";
            if (r.promotedFromQueue) {
                std::cout << "Queued vehicle " << r.promotedPlate
                          << " has been admitted automatically "
                             "(entry hour " << r.promotedHour << ").\n";
            }
            break;

        case ExitResult::NOT_PARKED:
            std::cout << "Vehicle has not entered. "
                         "Please enter the correct plate number.\n";
            break;
    }
}

void handleCancel(ParkingLot& lot) {
    std::cout << "Enter plate number: ";
    std::string plate = readLine();

    if (!isValidHKPlate(plate)) {
        std::cout << "Invalid HK plate format. "
                     "Expected 2-3 uppercase letters followed by 1-4 digits.\n";
        return;
    }

    CancelResult r = lot.cancelQueue(plate);

    switch (r.status) {
        case CancelResult::SUCCESS:
            std::cout << "Queue status for " << r.plate
                      << " cancelled. Remaining queued vehicles: "
                      << r.queueSize << ".\n";
            break;

        case CancelResult::NOT_IN_QUEUE:
            std::cout << "This vehicle is not in the waiting queue.\n";
            break;
    }
}

} // namespace

namespace UserMenu {

void run(ParkingLot& lot) {
    bool running = true;
    while (running) {
        printUserMenu();
        int choice = readInt();

        if (choice == MENU_EOF) {
            std::cout << "\nInput closed. Returning to main menu.\n";
            return;
        }

        switch (choice) {
            case 1: handleEnter(lot); break;
            case 2: handleExit(lot); break;
            case 3: handleCancel(lot); break;
            case 0: running = false; break;
            default: std::cout << "Invalid choice. Please enter 0-3.\n"; break;
        }
    }
}

} // namespace UserMenu