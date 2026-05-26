#include "FitnessCenter/fitness_center.h"
#include <iostream>
#include <string>
#include <limits>
#include <sstream>

// Human-friendly helper: read a line of text (allows spaces)
static std::string readLine(const std::string &prompt) {
    std::string s;
    std::cout << prompt;
    std::getline(std::cin, s);
    return s;
}

// Template helper: prompt for numeric values and validate input
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
        std::cout << "Valoare invalidă. Încearcă din nou.\n";
    }
}

// operator<< overloads to make display/printing easy and consistent
static std::ostream& operator<<(std::ostream &os, const Subscription &s) {
    os << "Subscription(type=" << s.getType() << ", price=" << s.getPrice() << ", validUntil=" << s.getValidation() << ")";
    return os;
}

static std::ostream& operator<<(std::ostream &os, const Member &m) {
    os << "Member[ID=" << m.getId() << ", Name=" << m.getName() << ", Age=" << m.getAge()
       << ", Contact=" << m.getEmail() << "|" << m.getPhoneNumber() << ", Sub=" << m.getSubscriptionType() << "]";
    return os;
}

static std::ostream& operator<<(std::ostream &os, const Trainer &t) {
    os << "Trainer[ID=" << t.getId() << ", Name=" << t.getName() << ", Age=" << t.getAge()
       << ", Contact=" << t.getEmail() << "|" << t.getPhoneNumber() << ", Spec=]"; // specialization is not exposed by a getter
    return os;
}

// Simple, human-friendly menu-driven CLI. Uses only public APIs of existing classes.
int main(int argc, char** argv) {
    // Default filename (can be overridden by CLI)
    std::string filename = "gym.txt";

    // Quick CLI arg parsing: accept `--file <path>` or a single filename as first arg
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if ((a == "--file" || a == "-f") && i + 1 < argc) {
            filename = argv[++i];
        } else if (i == 1) {
            filename = a; // first positional argument is treated as filename
        }
    }

    // Create fitness center using the filename as a default name (harmless)
    Fitness_Center center("MyGym");

    // Try loading existing data if available (Fitness_Center handles errors internally)
    center.loadFromFile(filename);

    // Main loop: menu
    while (true) {
        std::cout << "\n=== GYM APP ===\n";
        std::cout << "1) List all (members & trainers)\n";
        std::cout << "2) Add member\n";
        std::cout << "3) Add trainer\n";
        std::cout << "4) Save to file\n";
        std::cout << "5) Load from file\n";
        std::cout << "0) Exit\n";

        int choice = promptNumber<int>("Select option: ");

        if (choice == 0) {
            std::cout << "La revedere!\n";
            break;
        }

        if (choice == 1) {
            // Use existing displayAll implementation
            center.displayAll();
        }
        else if (choice == 2) {
            std::cout << "Adaugă membru (următorii pași):\n";
            int id = promptNumber<int>("  ID (numar): ");
            std::string name = readLine("  Name: ");
            int age = promptNumber<int>("  Age: ");
            std::string email = readLine("  Email: ");
            std::string phone = readLine("  Phone: ");
            std::string subType = readLine("  Subscription type (Basic/Premium/Student): ");

            Subscription s(subType, "N/A");
            Member m(id, name, age, email, phone, s);
            center.addMember(m);
            std::cout << "Membru adăugat: " << m << "\n";
        }
        else if (choice == 3) {
            std::cout << "Adaugă trainer:\n";
            std::string name = readLine("  Name: ");
            int age = promptNumber<int>("  Age: ");
            std::string email = readLine("  Email: ");
            std::string phone = readLine("  Phone: ");
            std::string spec = readLine("  Specialization: ");
            int id = promptNumber<int>("  Trainer ID (numar): ");

            Trainer t(name, age, email, phone, spec, id);
            center.addTrainer(t);
            std::cout << "Trainer adăugat: " << t << "\n";
        }
        else if (choice == 4) {
            std::string out = readLine("Filename to save (enter for default: " + filename + "): ");
            if (!out.empty()) filename = out;
            center.saveToFile(filename);
            std::cout << "Saved to " << filename << "\n";
        }
        else if (choice == 5) {
            std::string inf = readLine("Filename to load (enter for default: " + filename + "): ");
            if (!inf.empty()) filename = inf;
            center.loadFromFile(filename);
        }
        else {
            std::cout << "Opțiune necunoscută. Încearcă din nou." << std::endl;
        }
    }

    return 0;
}