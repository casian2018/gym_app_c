#include "Fitness_Center.h"
#include <utility>
#include <string>
#include <fstream>
#include <iostream>
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

    for (int i=0 ; i<members.size() ; i++) {
        file << "M " << members[i]->getName() << " " << members[i]->getAge() << " " << members[i]->getEmail() << " " << members[i]->getPhoneNumber() << "\n";
    }
}



