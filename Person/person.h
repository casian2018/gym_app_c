#ifndef PERSON_H
#define PERSON_H
#include <string>

/// @class Person
/// @brief Abstract base class for persons in the gym.
/// 
/// Defines common attributes and methods for any person
/// (members, trainers): name, age, email, phone.
class Person {
protected:
    std::string name;
    int age;
    std::string email;
    std::string phoneNumber;

public:
    /// @brief Constructor with parameters for Person.
    /// @param name - Person's name
    /// @param age - Person's age
    /// @param email - Person's email
    /// @param phoneNumber - Person's phone number
    Person( std::string,int, std::string, std::string);
    
    /// @brief Virtual destructor (needed for polymorphism).
    virtual ~Person() = default;
    
    /// @brief Pure virtual method for display.
    /// Must be implemented by derived classes (Member, Trainer).
    virtual void displayInfo() const=0;

    /// @brief Returns person's age.
    int getAge() const;
    
    /// @brief Returns person's name.
    std::string getName() const;
    
    /// @brief Returns person's email.
    std::string getEmail() const;
    
    /// @brief Returns person's phone number.
    std::string getPhoneNumber() const;
};

#endif
