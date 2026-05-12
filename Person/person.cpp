#include "Person.h"
#include <string>
#include <utility>

Person::Person(std::string n,int a,std::string e,std::string p) : name(std::move(n)), age(a), email(std::move(e)), phoneNumber(std::move(p)) {}

int Person::getAge() const {
    return age;
}

std::string Person::getName() const {
    return name;
}


std::string Person::getEmail() const {
    return email;
}

std::string Person::getPhoneNumber() const {
    return phoneNumber;
}
