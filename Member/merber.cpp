#include <iostream>
#include "member.h"
using namespace std;

Member::Member(int id, std::string name, std::string email, std::string phone, std::string membershipType) : memberId(id) {}

void Member::displayInfo() const {
        cout << "--------------------------" << endl;
        cout << "MEMBER PROFILE" << endl;
        cout << "ID: " << memberId << endl;
        cout << "Name: " << name << endl; // name e din Person
        cout << "Age: " << age << endl;   // age e din Person
        cout << "Membership Type: " << membershipType << endl;
        cout << "Contact: " << email << " | " << phoneNumber << endl;
        cout << "--------------------------" << endl;
}