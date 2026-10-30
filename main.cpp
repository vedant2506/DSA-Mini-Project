
#include "TaskManager.h"
#include "UI.h"

#include <iostream>
#include <string>

int main() {
    TaskManager manager;
    manager.loadFromFile();

    showWelcome();

    bool running = true;

    while (running) {
        showMenu();

        std::string input;
        if (!std::getline(std::cin, input))
            break;

        int choice = -1;

        try {
            std::size_t used = 0;
            choice = std::stoi(input, &used);

            if (input.find_first_not_of(" \t\r\n") ==
                std::string::npos ||
                input.find_last_not_of(" \t\r\n") + 1 != used) {
                choice = -1;
            }
        } catch (...) {
            choice = -1;
        }

        switch (choice) {
            case 1:
                manager.addTask();
                waitForReturn();
                break;

            case 2:
                manager.viewAllTasks();
                waitForReturn();
                break;

            case 3:
                manager.viewUrgentTasks();
                waitForReturn();
                break;

            case 4:
                manager.markTaskCompleted();
                waitForReturn();
                break;

            case 5:
                manager.deleteTask();
                waitForReturn();
                break;

            case 6:
                manager.saveToFile();
                std::cout << "\nTasks saved successfully.\n";
                waitForReturn();
                break;

            case 0:
                manager.saveToFile();
                showGoodbye();
                running = false;
                break;

            default:
                std::cout << "\nInvalid option. Please try again.\n";
                waitForReturn();
                break;
        }
    }

    return 0;
}
