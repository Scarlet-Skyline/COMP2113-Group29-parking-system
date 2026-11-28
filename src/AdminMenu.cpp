// AdminMenu.cpp
#include "AdminMenu.h"
#include "AdminAuth.h"
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdlib>

namespace
{

    const int MENU_EOF = -1;
    const int MENU_INVALID = -999;

    void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    // 等待用户按 0 返回
    void waitForReturn()
    {
        std::cout << "\nPress 0 to return to menu: ";
        std::string line;
        while (std::getline(std::cin, line))
        {
            if (line == "0")
                break;
            std::cout << "Please press 0 to return to menu: ";
        }
    }

    // Read a line of integers; Enter non numeric values to return MENU-INVALID, EOF to return MENU-EOF
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

    // read a string
    std::string readLine()
    {
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    std::string formatTimestamp(std::time_t t)
    {
        if (t == 0)
            return "-";
        std::tm tmv{};
#ifdef _WIN32
        localtime_s(&tmv, &t);
#else
        localtime_r(&t, &tmv);
#endif
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tmv);
        return std::string(buf);
    }

    void printAdminMenu()
    {
        std::cout << "\033[1;31m[ ADMIN MODE ]\033[0m\n";
        std::cout << "----------------------------------------\n";
        std::cout << "\n========== Administrator Menu ==========\n";
        std::cout << "1. View parked vehicles & waiting queue\n";
        std::cout << "2. View exit log (sorted by exit time)\n";
        std::cout << "3. Clear exit log\n";
        std::cout << "0. Return to main menu\n";
        std::cout << "----------------------------------------\n";
        std::cout << "Choice: ";
    }

    void showParkedVehicles(const ParkingLot &lot)
    {
        std::cout << "\n--- Parking Status ---\n";
        std::cout << "Total spaces    : " << lot.capacity() << "\n";
        std::cout << "Parked vehicles : " << lot.parkedCount() << "\n";
        std::cout << "Remaining spaces: " << lot.remainingSpaces() << "\n";
        std::cout << "Waiting queue   : " << lot.queueSize() << "\n";

        auto parked = lot.getParkedSnapshot();
        std::cout << "\n--- Parked Vehicles ---\n";
        if (parked.empty())
        {
            std::cout << "(none)\n";
        }
        else
        {
            std::cout << std::left << std::setw(12) << "Plate"
                      << "Entry Hour\n";
            for (const auto &info : parked)
            {
                std::cout << std::left << std::setw(12) << info.plate
                          << info.hour << "\n";
            }
        }

        auto queued = lot.getWaitingSnapshot();
        std::cout << "\n--- Waiting Queue (FIFO) ---\n";
        if (queued.empty())
        {
            std::cout << "(none)\n";
        }
        else
        {
            std::cout << std::left << std::setw(12) << "Plate"
                      << "Queued Hour\n";
            for (const auto &info : queued)
            {
                std::cout << std::left << std::setw(12) << info.plate
                          << info.hour << "\n";
            }
        }
    }

    void showExitLog(const ParkingLot &lot)
    {
        std::cout << "\n--- Exit Log (earliest exit first) ---\n";
        std::vector<ExitRecord> records = lot.getExitLog().sortedByExitTime();
        if (records.empty())
        {
            std::cout << "(log is empty)\n";
            return;
        }
        std::cout << std::left
                  << std::setw(12) << "Plate"
                  << std::setw(12) << "Entry Hour"
                  << std::setw(12) << "Exit Hour"
                  << std::setw(10) << "Duration"
                  << "Fee (HKD)\n";
        for (const auto &r : records)
        {
            const double hours = r.parkedSeconds() / 3600.0;
            std::cout << std::left
                      << std::setw(12) << r.licensePlate
                      << std::setw(20) << formatTimestamp(r.entryTime)
                      << std::setw(20) << formatTimestamp(r.exitTime)
                      << std::setw(10) << std::fixed << std::setprecision(2) << hours
                      << std::fixed << std::setprecision(2) << r.fee << "\n";
            std::cout.unsetf(std::ios::floatfield);
        }
    }

    void clearExitLog(ParkingLot &lot)
    {
        std::cout << "Are you sure you want to clear the exit log? (y/n): ";
        std::string ans = readLine();
        if (ans == "y" || ans == "Y")
        {
            lot.clearExitLog();
            std::cout << "Exit log cleared.\n";
        }
        else
        {
            std::cout << "Cancelled.\n";
        }
    }

} // namespace

namespace AdminMenu
{

    void run(ParkingLot &lot)
    {
        std::cout << "\nEnter administrator password: ";
        std::string pwd = readLine();
        if (!AdminAuth::verify(pwd))
        {
            std::cout << "Wrong password. Access denied.\n";
            waitForReturn();
            return;
        }
        // std::cout << "Access granted.\n";

        bool running = true;
        while (running)
        {
            clearScreen();
            printAdminMenu();
            const int choice = readInt();

            if (choice == MENU_EOF)
            {
                std::cout << "\nInput closed. Returning to main menu.\n";
                return;
            }

            switch (choice)
            {
            case 1:
                clearScreen();
                showParkedVehicles(lot);
                waitForReturn();
                break;
            case 2:
                clearScreen();
                showExitLog(lot);
                waitForReturn();
                break;
            case 3:
                clearScreen();
                clearExitLog(lot);
                waitForReturn();
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

} // namespace AdminMenu