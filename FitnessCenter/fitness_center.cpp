#include "Fitness_Center.h"
#include <utility>
#include <string>
#include <fstream>
#include <iostream>
#include "../Subscription/subscription.h"
const std::string Gym = "../gym.txt";
Fitness_Center::Fitness_Center(std::string n) : gymName(std::move(n)) {}

void Fitness_Center::addMember(const Member &m) {
    members.push_back(std::make_shared<Member>(m));
}

void Fitness_Center::addTrainer(const Trainer &t) {
    trainers.push_back(std::make_shared<Trainer>(t));
}


void Fitness_Center::saveToFile(const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cout<<"Error";
        return;
    }

    for (const auto& m : members) {
        file << "M "<< m->getId() << " "<<m->getName() << " " << m->getAge() << " " << m->getEmail() << " " << m->getPhoneNumber() <<" "<< m->getSubscriptionType() << "\n\n";
    }

    for (const auto& t : trainers) {
        file << "T " <<t->getId() <<" "<< t->getName() << " " << t->getAge() << " " << t->getEmail() << " " << t->getPhoneNumber() << "\n";
    }
}

void Fitness_Center::loadFromFile(const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cout<<"Error";
        return;
    }
    members.clear();
    trainers.clear();

    char type;

    while (file >> type) {
        if (type == 'M') {
            int id,age;
            std::string name,email,phone,subType;

            file >> id >> name >> age >> email >> phone >> subType;

            Subscription subTemp(subType, "N/A");
            Member newMem(id, name, age, email, phone, subTemp);
            addMember(newMem);
        }
        else if (type == 'T') {
            int age;
            std::string name, email, phone, specialization;
            int tId;

            file >> name >> age >> email >> phone >> specialization >> tId;
            Trainer newTr(name, age, email, phone, specialization, tId);

            addTrainer(newTr);
        }

    }
    std::cout << "Data has loaded succesfuly" << filename << "!\n";
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








