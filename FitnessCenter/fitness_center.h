#ifndef FITNESS_CENTER_H
#define FITNESS_CENTER_H
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include "../Member/Member.h"
#include "../Trainer/Trainer.h"
class Fitness_Center {
private:
    std::vector<std::shared_ptr<Member>> members;
    std::vector<std::shared_ptr<Trainer>> trainers;
    std::string gymName;

public:
    explicit Fitness_Center(std::string);
    void addMember(const Member& m);
    void addTrainer(const Trainer& t);
    void displayAll() const;
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

};

#endif
