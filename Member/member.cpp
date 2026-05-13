#include <iostream>
#include "member.h"
#include <utility>

using namespace std;


Member::Member(int id, std::string name, int age, std::string email, std::string phone, Subscription membershipType)
    : Person(std::move(name), age, std::move(email), std::move(phone)), memberId(id), sub(membershipType) {}

void Member::displayInfo() const {
        cout << "--------------------------" << endl;
        cout << "MEMBER PROFILE" << endl;
        cout << "ID: " << memberId << endl;
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;

        // Aici trebuie să apelezi un getter din obiectul tău 'sub' (Subscription)
        // De exemplu, dacă Subscription are o funcție getName() sau getType(), o pui așa:
        // cout << "Membership Type: " << sub.getType() << endl; // DE-COMENTEAZĂ ȘI PUNE FUNCȚIA CORECTĂ

        cout << "Contact: " << email << " | " << phoneNumber << endl;
        cout << "--------------------------" << endl;
}

int Member::getId() const {
    return memberId;
}
