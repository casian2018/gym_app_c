#ifndef TRAINER_H
#define TRAINER_H
#include <string>
#include "../Person/Person.h"
class Trainer : public Person{
private:
    std::string specialization;
    int trainerId;

public:
    Trainer(std::string,int,std::string,std::string, std::string, int);

    void displayInfo() const override;
    int getId() const {return trainerId;}

};

#endif
