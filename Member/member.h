#ifndef MEMBER_H
#define MEMBER_H
#include "../Person/person.h"
#include "../Subscription/subscription.h"
#include <string>

/// @class Member
/// @brief Class representing a gym member.
/// 
/// Inherits from Person and adds unique ID and subscription type.
/// Each member has a subscription (Basic, Premium, Student).
class Member : public Person {
protected:
    long long memberId;
    Subscription sub;

public:
    /// @brief Constructor with explicit ID.
    /// @param id - Member's unique ID
    /// @param name - Member's name
    /// @param age - Member's age
    /// @param email - Member's email
    /// @param phone - Member's phone number
    /// @param membershipType - Subscription type (Basic, Premium, Student)
    Member(long long id, std::string name, int age, std::string email, std::string phone, Subscription membershipType);
    
    /// @brief Constructor without ID (ID is auto-generated).
    Member(std::string name, int age, std::string email, std::string phone, Subscription membershipType);

    /// @brief Displays member information.
    void displayInfo() const override;
    
    /// @brief Returns member's unique ID.
    long long getId() const;
    
    /// @brief Returns subscription type as string.
    std::string getSubscriptionType() const { return sub.getType(); }
    
    /// @brief Returns complete subscription object.
    const Subscription& getSubscription() const { return sub; }
    
    /// @brief Changes member's subscription.
    void setSubscription(const Subscription& membershipType);
};

#endif