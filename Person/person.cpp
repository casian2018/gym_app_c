#include "Person.h"
#include <string>
#include <utility>

/// @brief Constructor - initializes a Person with name, age, email, and phone.
Person::Person(std::string n,int a,std::string e,std::string p) : name(std::move(n)), age(a), email(std::move(e)), phoneNumber(std::move(p)) {}

/// @brief Returns person's age.
int Person::getAge() const {
    return age;
}

/// @brief Returns person's name.
std::string Person::getName() const {
    return name;
}

/// @brief Returns person's email.
std::string Person::getEmail() const {
    return email;
}

/// @brief Returns person's phone number.
std::string Person::getPhoneNumber() const {
    return phoneNumber;
}
