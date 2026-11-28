// main.cpp
// COMP2113 Group Project: Parking Lot Management System
// Main program entry: shows splash screens, loads persistent data,
// runs the top-level menu, delegates to administrator/user menus,
// and saves data before exit.

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "include/Constants.h"
#include "include/ParkingLot.h"
#include "include/Persistence.h"
#include "include/AdminMenu.h"
#include "include/UserMenu.h"

namespace
{

    const int SPLASH_DELAY_SECONDS = 2; // each splash page stays 2 seconds

    void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void sleepForSeconds(int seconds)
    {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    }

    /**
     * @brief Check whether the splash animation should be shown.
     *        Setting the environment variable SKIP_SPLASH=1 disables it,
     *        which is useful for automated testing.
     * @return true if the splash screen should be displayed.
     */
    bool splashEnabled()
    {
        const char *env = std::getenv("SKIP_SPLASH");
        if (env != nullptr && std::string(env) == "1")
        {
            return false;
        }
        return true;
    }

    /**
     * @brief Show the opening animation:
     *        Page 1: "Hi :)" with an ASCII smiley face, shown for 2 seconds.
     *        Page 2: "Welcome to Use Parking Lot System", shown for 2 seconds.
     */
    void showSplashScreen()
    {
        if (!splashEnabled())
        {
            return;
        }

        // ---------- Page 1: Hi + smiley face ----------
        clearScreen();
        std::cout << "\n\n\n";
        std::cout << "            Hi  :)\n";
        std::cout << "\n";
        std::cout << "            *****     \n";
        std::cout << "          *       *   \n";
        std::cout << "         *  O   O  *  \n";
        std::cout << "          *  \\_/ *   \n";
        std::cout << "            *****     \n";
        std::cout << std::flush;
        sleepForSeconds(SPLASH_DELAY_SECONDS);

        // ---------- Page 2: welcome message ----------
        clearScreen();
        std::cout << "\n\n\n";
        std::cout << "        Welcome to Use Parking Lot System\n";
        std::cout << std::flush;
        sleepForSeconds(SPLASH_DELAY_SECONDS);

        // Clear before showing the main menu
        clearScreen();
    }

    void printMainMenu(const ParkingLot &lot)
    {

        clearScreen();

        std::cout << "\n";
        std::cout << "        \033[34m    .--.   \033[0m\n";
        std::cout << "        \033[34m .-(    ). \033[0m\n";
        std::cout << "        \033[34m(___.__)__)\033[0m\n\n";

        std::cout << "========================================\n";
        std::cout << "Remaining parking spaces: " << lot.remainingSpaces() << "\n";
        std::cout << "Vehicles in waiting queue: " << lot.queueSize() << "\n";
        std::cout << "----------------------------------------\n";
        std::cout << "1. Administrator mode\n";
        std::cout << "2. User mode\n";
        std::cout << "0. Exit\n";
        std::cout << "----------------------------------------\n";
        std::cout << "Please enter your choice: ";

        std::cout << "\n\n";
        std::cout << "                        \033[33m    __      \033[0m\n";
        std::cout << "                        \033[33m  _/  \\_    \033[0m\n";
        std::cout << "                        \033[33m (o)---(o)  \033[0m\n";
        std::cout << "-------------------------------------------------------------------------\n";
    }

    /**
     * @brief Read an integer choice from one line of input.
     * @return The parsed integer, -1 if input is closed/EOF,
     *         or -999 if the input is not a valid integer.
     */
    int readInt()
    {
        std::string line;
        if (!std::getline(std::cin, line))
        {
            return -1; // EOF or input stream closed
        }

        try
        {
            std::size_t pos = 0;
            int value = std::stoi(line, &pos);

            // Reject inputs such as "1abc" or "1 2"
            if (pos != line.size())
            {
                return -999;
            }
            return value;
        }
        catch (...)
        {
            return -999;
        }
    }

    /**
     * @brief Save data and print a warning if saving fails.
     * @param lot The parking lot to save.
     */
    void saveQuietly(const ParkingLot &lot)
    {
        if (!Persistence::save(lot, DATA_FILE))
        {
            std::cerr << "[Warning] Failed to save parking lot data to \""
                      << DATA_FILE << "\".\n";
        }
    }

} // namespace

int main()
{
    // ---------- Opening animation ----------
    showSplashScreen();

    // ---------- Load persistent data ----------
    ParkingLot lot(DEFAULT_CAPACITY);

    if (!Persistence::load(lot, DATA_FILE))
    {
        std::cout << "[Info] No existing data loaded. "
                  << "Starting with an empty parking lot.\n";
    }
    else
    {
        std::cout << "[Info] Parking lot data loaded successfully.\n";
    }

    bool running = true;

    while (running)
    {
        printMainMenu(lot);

        int choice = readInt();

        if (choice == -1)
        {
            std::cout << "\nInput closed. Exiting program.\n";
            break;
        }

        switch (choice)
        {
        case 1:
            // Administrator mode: password checking is handled inside AdminMenu.
            clearScreen();
            std::cout << "\033[1;31m[ Admin Mode ]\033[0m\n\n";
            AdminMenu::run(lot);
            saveQuietly(lot);
            break;

        case 2:
            // User mode: entry/exit/queue cancellation operations.
            clearScreen();
            std::cout << "\033[1;32m[ User Mode ]\033[0m\n\n";
            UserMenu::run(lot);
            saveQuietly(lot);
            break;

        case 0:
            running = false;
            break;

        default:
            std::cout << "Invalid choice. Please enter 0, 1, or 2.\n";
            break;
        }
    }

    // ---------- Final save before exit ----------
    if (Persistence::save(lot, DATA_FILE))
    {
        std::cout << "Parking lot data saved. Goodbye.\n";
    }
    else
    {
        std::cerr << "[Error] Failed to save parking lot data before exit.\n";
        return 1;
    }

    return 0;
}