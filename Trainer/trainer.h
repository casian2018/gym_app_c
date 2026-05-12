#ifndef TRAINER_H
#define TRAINER_H
#include <string>
#include "../Person/Person.h"
class Trainer : public Person{
private:
    std::string specialization;
    std::string trainerId;

public:
    Trainer(std::string,int,std::string,std::string, std::string, std::string);

    void displayInfo() const override;

};

#endif
