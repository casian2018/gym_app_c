#ifndef TRAINER_H
#define TRAINER_H
#include <string>
#include "../Person/Person.h"

/// @class Trainer
/// @brief Class representing a gym trainer.
/// 
/// Inherits from Person and adds unique ID and specialization
/// (e.g., Fitness, Yoga, Powerlifting).
class Trainer : public Person{
private:
    /// @brief Trainer's specialization (e.g., Fitness, Yoga, Powerlifting).
    std::string specialization;
    
    /// @brief Trainer's unique ID.
    long long trainerId;

public:
    /// @brief Constructor with explicit ID.
    /// @param id - Trainer's unique ID
    /// @param name - Trainer's name
    /// @param age - Trainer's age
    /// @param email - Trainer's email
    /// @param phone - Trainer's phone number
    /// @param specialization - Trainer's specialization
    Trainer(long long id, std::string name, int age, std::string email, std::string phone, std::string specialization);
    
    /// @brief Constructor without ID (ID is auto-generated).
    Trainer(std::string name, int age, std::string email, std::string phone, std::string specialization);

    /// @brief Displays trainer information.
    void displayInfo() const override;
    
    /// @brief Returns trainer's unique ID.
    long long getId() const {return trainerId;}
    
    /// @brief Returns trainer's specialization.
    std::string getSpecialization() const { return specialization; }

};

#endif
