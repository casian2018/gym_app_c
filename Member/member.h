#ifndef MEMBER_H
#define MEMBER_H
#include "../Person/person.h"
#include "../Subscription/subscription.h"
#include <string>

class Member : public Person {
protected:
    int memberId;
    Subscription sub;

public:

    Member(int id, std::string name, int age, std::string email, std::string phone, Subscription membershipType);

    void displayInfo() const override;
};

#endif