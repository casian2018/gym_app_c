#ifndef MEMBER_H
#define MEMBER_H
#include "../Person/person.h"
#include "../Subscription/subscription.h"
#include <string>

class Member : public Person {
protected:
    long long memberId;
    Subscription sub;

public:

    Member(long long id, std::string name, int age, std::string email, std::string phone, Subscription membershipType);
    Member(std::string name, int age, std::string email, std::string phone, Subscription membershipType);

    void displayInfo() const override;
    long long getId() const;
    std::string getSubscriptionType() const { return sub.getType(); }
    const Subscription& getSubscription() const { return sub; }
    void setSubscription(const Subscription& membershipType);
};

#endif