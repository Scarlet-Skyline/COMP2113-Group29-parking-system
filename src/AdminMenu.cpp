// AdminMenu.cpp
#include "AdminMenu.h"
#include "AdminAuth.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

namespace {

const int MENU_EOF     = -1;
const int MENU_INVALID = -999;

//Read a line of integers; Enter non numeric values to return MENU-INVALID, EOF to return MENU-EOF
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

// read a string
std::string readLine() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void printAdminMenu() {
    std::cout << "\n========== Administrator Menu ==========\n";
    std::cout << "1. View parked vehicles & waiting queue\n";
    std::cout << "2. View exit log (sorted by exit time)\n";
    std::cout << "3. Clear exit log\n";
    std::cout << "0. Return to main menu\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Choice: ";
}

void showParkedVehicles(const ParkingLot& lot) {
    std::cout << "\n--- Parking Status ---\n";
    std::cout << "Total spaces    : " << lot.capacity() << "\n";
    std::cout << "Parked vehicles : " << lot.parkedCount() << "\n";
    std::cout << "Remaining spaces: " << lot.remainingSpaces() << "\n";
    std::cout << "Waiting queue   : " << lot.queueSize() << "\n";

    auto parked = lot.getParkedSnapshot();
    std::cout << "\n--- Parked Vehicles ---\n";
    if (parked.empty()) {
        std::cout << "(none)\n";
    } else {
        std::cout << std::left << std::setw(12) << "Plate"
                  << "Entry Hour\n";
        for (const auto& info : parked) {
            std::cout << std::left << std::setw(12) << info.plate
                      << info.hour << "\n";
        }
    }

    auto queued = lot.getWaitingSnapshot();
    std::cout << "\n--- Waiting Queue (FIFO) ---\n";
    if (queued.empty()) {
        std::cout << "(none)\n";
    } else {
        std::cout << std::left << std::setw(12) << "Plate"
                  << "Queued Hour\n";
        for (const auto& info : queued) {
            std::cout << std::left << std::setw(12) << info.plate
                      << info.hour << "\n";
        }
    }
}

void showExitLog(const ParkingLot& lot) {
    std::cout << "\n--- Exit Log (earliest exit first) ---\n";
    std::vector<ExitRecord> records = lot.getExitLog().sortedByExitTime();
    if (records.empty()) {
        std::cout << "(log is empty)\n";
        return;
    }
    std::cout << std::left
              << std::setw(12) << "Plate"
              << std::setw(12) << "Entry Hour"
              << std::setw(12) << "Exit Hour"
              << std::setw(10) << "Duration"
              << "Fee (HKD)\n";
    for (const auto& r : records) {
        int duration = (r.exitHour >= r.entryHour)
                       ? (r.exitHour - r.entryHour)
                       : (r.exitHour + 24 - r.entryHour);
        std::cout << std::left
                  << std::setw(12) << r.plate
                  << std::setw(12) << r.entryHour
                  << std::setw(12) << r.exitHour
                  << std::setw(10) << duration
                  << r.fee << "\n";
    }
}

void clearExitLog(ParkingLot& lot) {
    std::cout << "Are you sure you want to clear the exit log? (y/n): ";
    std::string ans = readLine();
    if (ans == "y" || ans == "Y") {
        lot.clearExitLog();
        std::cout << "Exit log cleared.\n";
    } else {
        std::cout << "Cancelled.\n";
    }
}

} // namespace

namespace AdminMenu {

void run(ParkingLot& lot) {
    std::cout << "\nEnter administrator password: ";
    std::string pwd = readLine();
    if (!AdminAuth::verify(pwd)) {
        std::cout << "Wrong password. Access denied.\n";
        return;
    }
    std::cout << "Access granted.\n";

    bool running = true;
    while (running) {
        printAdminMenu();
        int choice = readInt();

        if (choice == MENU_EOF) {
            std::cout << "\nInput closed. Returning to main menu.\n";
            return;
        }

        switch (choice) {
            case 1: showParkedVehicles(lot); break;
            case 2: showExitLog(lot); break;
            case 3: clearExitLog(lot); break;
            case 0: running = false; break;
            default: std::cout << "Invalid choice. Please enter 0-3.\n"; break;
        }
    }
}

} // namespace AdminMenu