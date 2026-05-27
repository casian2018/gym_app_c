#include "Fitness_Center.h"
#include <utility>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>
#include "../Subscription/subscription.h"

namespace {
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

std::string makeStorageFilename(const std::string& gymName) {
    return sanitizeGymName(gymName) + ".txt";
}
}

Fitness_Center::Fitness_Center(std::string n, std::string storageFile)
    : gymName(std::move(n)), storageFilename(std::move(storageFile)), loadingFromFile(false) {
    loadFromFile();
}

void Fitness_Center::cacheCurrentGym() {
    gymCache[gymName] = GymProfile{members, trainers, storageFilename};
}

void Fitness_Center::restoreGymProfile(const GymProfile& profile) {
    members = profile.members;
    trainers = profile.trainers;
    storageFilename = profile.storageFilename;
}

void Fitness_Center::cacheAppointments() {
    (void)0;
}

const std::string& Fitness_Center::getGymName() const {
    return gymName;
}

const std::string& Fitness_Center::getStorageFilename() const {
    return storageFilename;
}

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

void Fitness_Center::addMember(const Member &m) {
    members.push_back(std::make_shared<Member>(m));
    if (!loadingFromFile) {
        saveToFile();
    }
    cacheCurrentGym();
}

void Fitness_Center::addTrainer(const Trainer &t) {
    trainers.push_back(std::make_shared<Trainer>(t));
    if (!loadingFromFile) {
        saveToFile();
    }
    cacheCurrentGym();
}


void Fitness_Center::saveToFile() {
    saveToFile(storageFilename);
}

void Fitness_Center::saveToFile(const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        return;
    }

    for (const auto& m : members) {
        file << "M "<< m->getId() << " "<<m->getName() << " " << m->getAge() << " " << m->getEmail() << " " << m->getPhoneNumber() <<" "<< m->getSubscriptionType() << "\n\n";
    }

    for (const auto& t : trainers) {
        file << "T " << t->getId() << " " << t->getName() << " " << t->getAge() << " " << t->getEmail() << " " << t->getPhoneNumber() << " " << t->getSpecialization() << "\n";
    }

    for (const auto& a : appointments) {
        file << "A " << a.memberId << " " << a.trainerId << " " << a.timeSlot << "\n";
    }

    cacheCurrentGym();
}

void Fitness_Center::loadFromFile() {
    loadFromFile(storageFilename);
}

void Fitness_Center::loadFromFile(const std::string &filename) {
    storageFilename = filename;
    members.clear();
    trainers.clear();

    std::ifstream file(filename);

    if (!file.is_open()) {
        cacheCurrentGym();
        return;
    }

    loadingFromFile = true;

    char type;

    while (file >> type) {
        if (type == 'M') {
            long long id;
            int age;
            std::string name,email,phone,subType;

            file >> id >> name >> age >> email >> phone >> subType;

            Subscription subTemp(subType, "N/A");
            Member newMem(id, name, age, email, phone, subTemp);
            addMember(newMem);
        }
        else if (type == 'T') {
            long long tId;
            int age;
            std::string name, email, phone, specialization;

            file >> tId >> name >> age >> email >> phone >> specialization;
            Trainer newTr(tId, name, age, email, phone, specialization);

            addTrainer(newTr);
        }
        else if (type == 'A') {
            Appointment app{};
            file >> app.memberId >> app.trainerId >> app.timeSlot;
            appointments.push_back(app);
        }

    }
    loadingFromFile = false;
    cacheCurrentGym();
    }

void Fitness_Center::switchGym(const std::string& newGymName) {
    saveToFile();
    cacheCurrentGym();

    gymName = newGymName.empty() ? "MyGym" : newGymName;
    storageFilename = makeStorageFilename(gymName);

    const auto cached = gymCache.find(gymName);
    if (cached != gymCache.end()) {
        restoreGymProfile(cached->second);
        return;
    }

    members.clear();
    trainers.clear();

    std::ifstream file(storageFilename);
    if (file.is_open()) {
        loadFromFile(storageFilename);
    } else {
        saveToFile(storageFilename);
    }

    cacheCurrentGym();
}

bool Fitness_Center::removeMember(long long memberId) {
    auto before = members.size();
    members.erase(std::remove_if(members.begin(), members.end(), [memberId](const auto& member) {
        return member && member->getId() == memberId;
    }), members.end());
    if (members.size() != before) {
        appointments.erase(std::remove_if(appointments.begin(), appointments.end(), [memberId](const Appointment& a) {
            return a.memberId == memberId;
        }), appointments.end());
        saveToFile();
        return true;
    }
    return false;
}

bool Fitness_Center::removeTrainer(long long trainerId) {
    auto before = trainers.size();
    trainers.erase(std::remove_if(trainers.begin(), trainers.end(), [trainerId](const auto& trainer) {
        return trainer && trainer->getId() == trainerId;
    }), trainers.end());
    if (trainers.size() != before) {
        appointments.erase(std::remove_if(appointments.begin(), appointments.end(), [trainerId](const Appointment& a) {
            return a.trainerId == trainerId;
        }), appointments.end());
        saveToFile();
        return true;
    }
    return false;
}

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

void Fitness_Center::addAppointment(long long memberId, long long trainerId, const std::string& timeSlot) {
    appointments.push_back(Appointment{memberId, trainerId, timeSlot});
    saveToFile();
}

const std::vector<Fitness_Center::Appointment>& Fitness_Center::getAppointments() const {
    return appointments;
}

bool Fitness_Center::trainerIsBusy(long long trainerId) const {
    return std::any_of(appointments.begin(), appointments.end(), [trainerId](const Appointment& a) {
        return a.trainerId == trainerId;
    });
}

void Fitness_Center::clearTrainerAppointments(long long trainerId) {
    appointments.erase(std::remove_if(appointments.begin(), appointments.end(), [trainerId](const Appointment& a) {
        return a.trainerId == trainerId;
    }), appointments.end());
    saveToFile();
}


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

const std::vector<std::shared_ptr<Member>>& Fitness_Center::getMembers() const {
    return members;
}

const std::vector<std::shared_ptr<Trainer>>& Fitness_Center::getTrainers() const {
    return trainers;
}

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








