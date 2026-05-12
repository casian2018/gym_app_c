#include "Fitness_Center.h"
#include <utility>
#include <string>
Fitness_Center::Fitness_Center(std::string n) : gymName(std::move(n)) {}

void Fitness_Center::addMember(const Member &m) {
    members.push_back(std::make_shared<Member>(m));
}

void Fitness_Center::addTrainer(const Trainer &t) {
    trainers.push_back(std::make_shared<Trainer>(t));
}



