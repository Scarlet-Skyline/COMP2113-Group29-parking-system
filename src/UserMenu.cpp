// UserMenu.cpp
#include "UserMenu.h"
#include "PlateValidator.h"

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

namespace
{

    const int MENU_EOF = -1;
    const int MENU_INVALID = -999;

    int readInt()
    {
        std::string line;
        if (!std::getline(std::cin, line))
            return MENU_EOF;
        try
        {
            std::size_t pos = 0;
            int v = std::stoi(line, &pos);
            if (pos != line.size())
                return MENU_INVALID;
            return v;
        }
        catch (...)
        {
            return MENU_INVALID;
        }
    }

    std::string readLine()
    {
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    // Reading an hour from 0-23, illegally returning -1
    bool readDateTime(const std::string &prompt, DateTime &out)
    {
        std::cout << prompt << " (dd/mm/yyyy hhmm): ";
        std::string line = readLine();

        std::istringstream iss(line);
        char s1 = 0, s2 = 0;
        std::string timeStr;
        if (!(iss >> out.day >> s1 >> out.month >> s2 >> out.year >> timeStr))
        {
            return false;
        }
        if (s1 != '/' || s2 != '/')
            return false;
        if (out.day < 1 || out.day > 31)
            return false;
        if (out.month < 1 || out.month > 12)
            return false;
        if (out.year < 1970)
            return false;
        if (timeStr.size() != 4)
            return false;
        for (char c : timeStr)
        {
            if (!std::isdigit(static_cast<unsigned char>(c)))
                return false;
        }
        const int hhmm = std::stoi(timeStr);
        const int hh = hhmm / 100;
        const int mm = hhmm % 100;
        if (hh < 0 || hh > 23)
            return false;
        if (mm < 0 || mm > 59)
            return false;
        out.hour = hh;
        out.minute = mm;
        return true;
    }

    void printUserMenu()
    {
        std::cout << "\n============ User Menu ============\n";
        std::cout << "1. Vehicle entry\n";
        std::cout << "2. Vehicle exit\n";
        std::cout << "3. Cancel queue\n";
        std::cout << "0. Return to main menu\n";
        std::cout << "-----------------------------------\n";
        std::cout << "Choice: ";
    }

    void handleEnter(ParkingLot &lot)
    {
        std::cout << "Enter plate number: ";
        std::string plate = readLine();

        if (!validate_plate(normalize_plate(plate)))
        {
            std::cout << "Invalid HK plate format. ";
            return;
        }

        DateTime when{};
        if (!readDateTime("Enter entry time", when))
        {
            std::cout << "Invalid date/time format.\n";
            return;
        }

        EnterResult r = lot.enterVehicle(plate, when);

        switch (r.status)
        {
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

        case EnterResult::INVALID_PLATE:
            std::cout << "Invalid HK plate format. Entry denied.\n";
            break;
        }
    }

    void handleExit(ParkingLot &lot)
    {
        std::cout << "Enter plate number: ";
        std::string plate = readLine();

        if (!validate_plate(normalize_plate(plate)))
        {
            std::cout << "Invalid HK plate format. "
                         "Expected 2-3 uppercase letters followed by 1-4 digits.\n";
            return;
        }

        DateTime when{};
        if (!readDateTime("Enter exit time", when))
        {
            std::cout << "Invalid date/time format.\n";
            return;
        }

        ExitResult r = lot.exitVehicle(plate, when);

        switch (r.status)
        {
        case ExitResult::SUCCESS:
            std::cout << "Exit successful. "
                      << "Parking duration: " << r.durationHours << " hour(s). "
                      << "Fee: " << r.fee << " HKD. "
                      << "Remaining spaces: " << r.remainingSpaces << ".\n";
            if (r.promotedFromQueue)
            {
                std::cout << "Queued vehicle " << r.promotedPlate
                          << " has been admitted automatically "
                             "(entry hour "
                          << r.promotedHour << ").\n";
            }
            break;

        case ExitResult::NOT_PARKED:
            std::cout << "Vehicle has not entered. "
                         "Please enter the correct plate number.\n";
            break;
        }
    }

    void handleCancel(ParkingLot &lot)
    {
        std::cout << "Enter plate number: ";
        std::string plate = readLine();

        if (!validate_plate(normalize_plate(plate)))
        {
            std::cout << "Invalid HK plate format. ";
            return;
        }

        CancelResult r = lot.cancelQueue(plate);

        switch (r.status)
        {
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

namespace UserMenu
{

    void run(ParkingLot &lot)
    {
        bool running = true;
        while (running)
        {
            printUserMenu();
            int choice = readInt();

            if (choice == MENU_EOF)
            {
                std::cout << "\nInput closed. Returning to main menu.\n";
                return;
            }

            switch (choice)
            {
            case 1:
                handleEnter(lot);
                break;
            case 2:
                handleExit(lot);
                break;
            case 3:
                handleCancel(lot);
                break;
            case 0:
                running = false;
                break;
            default:
                std::cout << "Invalid choice. Please enter 0-3.\n";
                break;
            }
        }
    }

} // namespace UserMenu