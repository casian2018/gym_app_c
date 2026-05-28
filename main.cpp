#include "FitnessCenter/fitness_center.h"
#include <iostream>
#include <string>
#include <limits>
#include <sstream>

/// @brief Helper function to read a line of text (allows spaces and special characters).
static std::string readLine(const std::string &prompt) {
    std::string s;
    std::cout << prompt;
    std::getline(std::cin, s);
    return s;
}

/// @brief Template helper: prompts for numeric values and validates input.
/// Keeps asking until valid number is entered.
template<typename T>
T promptNumber(const std::string &prompt) {
    T value;
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) {
            // EOF or error: clear and continue
            std::cin.clear();
            continue;
        }
        std::istringstream iss(line);
        if (iss >> value) return value;
        std::cout << "Invalid value. Try again.\n";
    }
}

/// @brief operator<< for Subscription - makes printing easy with cout.
static std::ostream& operator<<(std::ostream &os, const Subscription &s) {
    os << "Subscription(type=" << s.getType() << ", price=" << s.getPrice() << ", validUntil=" << s.getValidation() << ")";
    return os;
}

/// @brief operator<< for Member - displays member info in compact format.
static std::ostream& operator<<(std::ostream &os, const Member &m) {
    os << "Member[ID=" << m.getId() << ", Name=" << m.getName() << ", Age=" << m.getAge()
       << ", Contact=" << m.getEmail() << "|" << m.getPhoneNumber() << ", Sub=" << m.getSubscriptionType() << "]";
    return os;
}

/// @brief operator<< for Trainer - displays trainer info in compact format.
static std::ostream& operator<<(std::ostream &os, const Trainer &t) {
    os << "Trainer[ID=" << t.getId() << ", Name=" << t.getName() << ", Age=" << t.getAge()
       << ", Contact=" << t.getEmail() << "|" << t.getPhoneNumber() << ", Spec=" << t.getSpecialization() << "]";
    return os;
}

/// @brief Main entry point - CLI menu-driven application.
/// Handles command-line arguments for file selection.
int main(int argc, char** argv) {
    // Parse command-line arguments: --file <path> or filename as first arg
    std::string filename = "gym.txt";
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if ((a == "--file" || a == "-f") && i + 1 < argc) {
            filename = argv[++i];
        } else if (i == 1) {
            filename = a; // first positional argument is filename
        }
    }

    // Create fitness center - loads automatically from file
    Fitness_Center center("MyGym", filename);

    // Main menu loop
    while (true) {
        std::cout << "\n=== GYM APP ===\n";
        std::cout << "1) List all (members & trainers)\n";
        std::cout << "2) Add member\n";
        std::cout << "3) Add trainer\n";
        std::cout << "4) Switch gym\n";
        std::cout << "0) Exit\n";

        int choice = promptNumber<int>("Select option: ");

        if (choice == 0) {
            std::cout << "Goodbye!\n";
            break;
        }

        if (choice == 1) {
            // Display all members and trainers
            center.displayAll();
        }
        else if (choice == 2) {
            // Add new member
            std::cout << "Add member (next steps):\n";
            std::string name = readLine("  Name: ");
            int age = promptNumber<int>("  Age: ");
            std::string email = readLine("  Email: ");
            std::string phone = readLine("  Phone: ");
            std::string subType = readLine("  Subscription type (Basic/Premium/Student): ");

            Subscription s(subType, "N/A");
            Member m(name, age, email, phone, s);
            center.addMember(m);
            std::cout << "Member added: " << m << "\n";
        }
        else if (choice == 3) {
            // Add new trainer
            std::cout << "Add trainer:\n";
            std::string name = readLine("  Name: ");
            int age = promptNumber<int>("  Age: ");
            std::string email = readLine("  Email: ");
            std::string phone = readLine("  Phone: ");
            std::string spec = readLine("  Specialization: ");

            Trainer t(name, age, email, phone, spec);
            center.addTrainer(t);
            std::cout << "Trainer added: " << t << "\n";
        }
        else if (choice == 4) {
            // Switch to different gym
            std::string newGym = readLine("New gym name: ");
            if (newGym.empty()) {
                std::cout << "Gym name cannot be empty.\n";
                continue;
            }

            center.switchGym(newGym);
            std::cout << "Switched to gym: " << newGym
                      << " (file: " << center.getStorageFilename() << ")\n";
        }
        else {
            std::cout << "Unknown option. Try again." << std::endl;
        }
    }

    // Save before exit
    center.saveToFile();

    return 0;
}