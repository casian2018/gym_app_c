#include "Fitness_Center.h"
#include <utility>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include "../Subscription/subscription.h"

namespace {
/// @brief Removes special characters from gym name, replaces spaces with underscores.
/// This ensures the gym name is safe for use as filename.
std::string sanitizeGymName(const std::string& value) {
    std::string result;
    result.reserve(value.size());

    for (unsigned char ch : value) {
        if (std::isalnum(ch)) {
            result.push_back(static_cast<char>(ch));
        } else if (std::isspace(ch) || ch == '-' || ch == '_') {
            result.push_back('_');
        }
    }

    if (result.empty()) {
        result = "gym";
    }

    return result;
}

/// @brief Creates storage filename from gym name (e.g., "FitnessMania" -> "FitnessMania.txt").
std::string makeStorageFilename(const std::string& gymName) {
    return sanitizeGymName(gymName) + ".txt";
}
}

/// @brief Constructor - initializes gym, loads gym list, then loads data from file.
Fitness_Center::Fitness_Center(std::string n, std::string storageFile)
    : gymName(std::move(n)), storageFilename(std::move(storageFile)), loadingFromFile(false) {
    loadGymList();
    loadFromFile();
}

/// @brief Caches current gym profile (saves to gymCache for quick switching).
void Fitness_Center::cacheCurrentGym() {
    gymCache[gymName] = GymProfile{members, trainers, storageFilename};
}

/// @brief Restores gym profile from cache.
void Fitness_Center::restoreGymProfile(const GymProfile& profile) {
    members = profile.members;
    trainers = profile.trainers;
    storageFilename = profile.storageFilename;
}

/// @brief Placeholder for caching appointments (can be extended).
void Fitness_Center::cacheAppointments() {
    (void)0;
}

/// @brief Returns current gym name.
const std::string& Fitness_Center::getGymName() const {
    return gymName;
}

/// @brief Returns current storage file name.
const std::string& Fitness_Center::getStorageFilename() const {
    return storageFilename;
}

/// @brief Returns list of all available gyms (from cache).
std::vector<std::string> Fitness_Center::getAvailableGyms() const {
    std::vector<std::string> names;
    names.reserve(gymCache.size() + 1);
    for (const auto &p : gymCache) {
        names.push_back(p.first);
    }
    if (std::find(names.begin(), names.end(), gymName) == names.end()) {
        names.push_back(gymName);
    }
    std::sort(names.begin(), names.end());
    names.erase(std::unique(names.begin(), names.end()), names.end());
    return names;
}

/// @brief Adds new member to current gym and saves to file.
void Fitness_Center::addMember(const Member &m) {
    members.push_back(std::make_shared<Member>(m));
    if (!loadingFromFile) {
        saveToFile();
    }
    cacheCurrentGym();
}

/// @brief Adds new trainer to current gym and saves to file.
void Fitness_Center::addTrainer(const Trainer &t) {
    trainers.push_back(std::make_shared<Trainer>(t));
    if (!loadingFromFile) {
        saveToFile();
    }
    cacheCurrentGym();
}

/// @brief Saves current gym to default storage file.
void Fitness_Center::saveToFile() {
    saveToFile(storageFilename);
}

/// @brief Saves current gym data to specified file.
/// File format: M(ember) line, T(rainer) line, A(ppointment) line.
void Fitness_Center::saveToFile(const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        return;
    }

    // Save all members (M = Member marker)
    for (const auto& m : members) {
        const auto& subscription = m->getSubscription();
        file << "M " << m->getId() << " " << m->getName() << " " << m->getAge() << " " << m->getEmail() << " "
             << m->getPhoneNumber() << " " << subscription.getType() << " " << subscription.getValidation() << " "
             << subscription.getPaymentStatus() << "\n";
    }

    // Save all trainers (T = Trainer marker)
    for (const auto& t : trainers) {
        file << "T " << t->getId() << " " << t->getName() << " " << t->getAge() << " " << t->getEmail() << " " << t->getPhoneNumber() << " " << t->getSpecialization() << "\n";
    }

    // Save all appointments (A = Appointment marker)
    for (const auto& a : appointments) {
        file << "A " << a.memberId << " " << a.trainerId << " " << a.timeSlot << "\n";
    }

    cacheCurrentGym();
}

/// @brief Loads current gym from default storage file.
void Fitness_Center::loadFromFile() {
    loadFromFile(storageFilename);
}

/// @brief Loads gym data from specified file.
/// Parses file format and rebuilds members, trainers, appointments lists.
void Fitness_Center::loadFromFile(const std::string &filename) {
    storageFilename = filename;
    members.clear();
    trainers.clear();
    appointments.clear();

    std::ifstream file(filename);

    if (!file.is_open()) {
        cacheCurrentGym();
        return;
    }

    loadingFromFile = true;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        char type = 0;
        iss >> type;

        // Parse Member line
        if (type == 'M') {
            long long id = 0;
            int age = 0;
            std::string name, email, phone, subType, validUntil, paymentStatus;
            iss >> id >> name >> age >> email >> phone >> subType >> validUntil;
            if (!(iss >> paymentStatus)) {
                paymentStatus = "Paid";
            }

            Subscription subTemp(subType, validUntil);
            subTemp.setPaymentStatus(paymentStatus);
            members.push_back(std::make_shared<Member>(id, name, age, email, phone, subTemp));
        } 
        // Parse Trainer line
        else if (type == 'T') {
            long long tId = 0;
            int age = 0;
            std::string name, email, phone, specialization;
            iss >> tId >> name >> age >> email >> phone >> specialization;
            trainers.push_back(std::make_shared<Trainer>(tId, name, age, email, phone, specialization));
        } 
        // Parse Appointment line
        else if (type == 'A') {
            Appointment app{};
            iss >> app.memberId >> app.trainerId >> app.timeSlot;
            if (app.memberId != 0 && app.trainerId != 0 && !app.timeSlot.empty()) {
                appointments.push_back(app);
            }
        }
    }

    loadingFromFile = false;
    cacheCurrentGym();
}

/// @brief Switches to different gym (or creates if not exists).
/// Saves current gym, then loads target gym from cache or file.
void Fitness_Center::switchGym(const std::string& newGymName) {
    saveToFile();
    cacheCurrentGym();

    gymName = newGymName.empty() ? "MyGym" : newGymName;
    storageFilename = makeStorageFilename(gymName);

    // Check if gym exists in cache
    const auto cached = gymCache.find(gymName);
    if (cached != gymCache.end()) {
        restoreGymProfile(cached->second);
        saveGymList();
        return;
    }

    members.clear();
    trainers.clear();

    // Try to load from file, or create new gym
    std::ifstream file(storageFilename);
    if (file.is_open()) {
        loadFromFile(storageFilename);
    } else {
        saveToFile(storageFilename);
    }

    cacheCurrentGym();
    saveGymList();
}

/// @brief Saves list of all available gyms to gyms_list.txt.
void Fitness_Center::saveGymList() {
    std::ofstream file("gyms_list.txt");
    if (!file.is_open()) {
        return;
    }
    for (const auto& p : gymCache) {
        file << p.first << "\n";
    }
    if (gymCache.find(gymName) == gymCache.end()) {
        file << gymName << "\n";
    }
}

/// @brief Loads gym list from gyms_list.txt.
void Fitness_Center::loadGymList() {
    std::ifstream file("gyms_list.txt");
    if (!file.is_open()) {
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            const std::string filename = makeStorageFilename(line);
            std::ifstream dataFile(filename);
            if (dataFile.is_open()) {
                dataFile.close();
                GymProfile profile;
                profile.storageFilename = filename;
                gymCache[line] = profile;
            }
        }
    }
}

/// @brief Removes member by ID and cleans up associated appointments.
bool Fitness_Center::removeMember(long long memberId) {
    auto before = members.size();
    members.erase(std::remove_if(members.begin(), members.end(), [memberId](const auto& member) {
        return member && member->getId() == memberId;
    }), members.end());
    if (members.size() != before) {
        // Also remove appointments for this member
        appointments.erase(std::remove_if(appointments.begin(), appointments.end(), [memberId](const Appointment& a) {
            return a.memberId == memberId;
        }), appointments.end());
        saveToFile();
        return true;
    }
    return false;
}

/// @brief Removes trainer by ID and cleans up associated appointments.
bool Fitness_Center::removeTrainer(long long trainerId) {
    auto before = trainers.size();
    trainers.erase(std::remove_if(trainers.begin(), trainers.end(), [trainerId](const auto& trainer) {
        return trainer && trainer->getId() == trainerId;
    }), trainers.end());
    if (trainers.size() != before) {
        // Also remove appointments for this trainer
        appointments.erase(std::remove_if(appointments.begin(), appointments.end(), [trainerId](const Appointment& a) {
            return a.trainerId == trainerId;
        }), appointments.end());
        saveToFile();
        return true;
    }
    return false;
}

/// @brief Updates member's subscription.
bool Fitness_Center::updateMemberSubscription(long long memberId, const Subscription& subscription) {
    for (auto& member : members) {
        if (member && member->getId() == memberId) {
            member->setSubscription(subscription);
            saveToFile();
            return true;
        }
    }
    return false;
}

/// @brief Adds new appointment between member and trainer.
void Fitness_Center::addAppointment(long long memberId, long long trainerId, const std::string& timeSlot) {
    appointments.push_back(Appointment{memberId, trainerId, timeSlot});
    saveToFile();
}

/// @brief Returns list of all appointments.
const std::vector<Fitness_Center::Appointment>& Fitness_Center::getAppointments() const {
    return appointments;
}

/// @brief Checks if trainer has any appointments.
bool Fitness_Center::trainerIsBusy(long long trainerId) const {
    return std::any_of(appointments.begin(), appointments.end(), [trainerId](const Appointment& a) {
        return a.trainerId == trainerId;
    });
}

/// @brief Clears all appointments for specified trainer.
void Fitness_Center::clearTrainerAppointments(long long trainerId) {
    appointments.erase(std::remove_if(appointments.begin(), appointments.end(), [trainerId](const Appointment& a) {
        return a.trainerId == trainerId;
    }), appointments.end());
    saveToFile();
}

/// @brief Displays all members and trainers in current gym.
void Fitness_Center::displayAll() const {
    std::cout << "=== " << gymName << " - All Members and Trainers ===\n";
    if (members.empty()) {
        std::cout << "No members available." << std::endl;
    } else {
        std::cout << "Members:\n";
        for (const auto &m : members) {
            if (m) m->displayInfo();
        }
    }

    if (trainers.empty()) {
        std::cout << "No trainers available." << std::endl;
    } else {
        std::cout << "Trainers:\n";
        for (const auto &t : trainers) {
            if (t) t->displayInfo();
        }
    }
}

/// @brief Returns list of all members.
const std::vector<std::shared_ptr<Member>>& Fitness_Center::getMembers() const {
    return members;
}

/// @brief Returns list of all trainers.
const std::vector<std::shared_ptr<Trainer>>& Fitness_Center::getTrainers() const {
    return trainers;
}

/// @brief Returns snapshot of gym data as formatted string lines.
std::vector<std::string> Fitness_Center::snapshotLines() const {
    std::vector<std::string> lines;

    lines.push_back("Gym: " + gymName);
    lines.push_back("Members: " + std::to_string(members.size()));
    for (const auto &m : members) {
        if (!m) {
            continue;
        }
        std::ostringstream out;
        out << "M " << m->getId() << " | " << m->getName() << " | age " << m->getAge()
            << " | " << m->getSubscriptionType() << " | " << m->getEmail() << " | " << m->getPhoneNumber();
        lines.push_back(out.str());
    }

    lines.push_back("Trainers: " + std::to_string(trainers.size()));
    for (const auto &t : trainers) {
        if (!t) {
            continue;
        }
        std::ostringstream out;
        out << "T " << t->getId() << " | " << t->getName() << " | age " << t->getAge()
            << " | " << t->getSpecialization() << " | " << t->getEmail() << " | " << t->getPhoneNumber();
        lines.push_back(out.str());
    }

    return lines;
}








