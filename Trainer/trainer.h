#ifndef TRAINER_H
#define TRAINER_H
#include <string>
#include "../Person/Person.h"
class Trainer : public Person{
private:
    std::string specialization;
    long long trainerId;

public:
    Trainer(long long id, std::string name, int age, std::string email, std::string phone, std::string specialization);
    Trainer(std::string name, int age, std::string email, std::string phone, std::string specialization);

    void displayInfo() const override;
    long long getId() const {return trainerId;}
    std::string getSpecialization() const { return specialization; }

};

#endif
