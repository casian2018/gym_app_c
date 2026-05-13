#include "Trainer.h"
#include <string>
#include <utility>
#include <iostream>

Trainer::Trainer(std::string n, int a, std::string e, std::string p,  std::string s,int tId) : Person(std::move(n), a, std::move(e), std::move(p)), specialization(std::move(s)), trainerId(tId)  {}

void Trainer::displayInfo() const {
        std::cout << "--------------------------" << std::endl;
        std::cout << "TRAINER PROFILE" << std::endl;
        std::cout << "ID: " << trainerId << std::endl;
        std::cout << "Name: " << name << std::endl; // name e din Person
        std::cout << "Age: " << age << std::endl;   // age e din Person
        std::cout << "Specialization: " << specialization << std::endl;
        std::cout << "Contact: " << email << " | " << phoneNumber << std::endl;
        std::cout << "--------------------------" << std::endl;
}
