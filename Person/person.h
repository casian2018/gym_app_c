#ifndef PERSON_H
#define PERSON_H
#include <string>
class Person {
protected:
    std::string name;
    int age;
    std::string email;
    std::string phoneNumber;

public:
    Person( std::string,int, std::string, std::string);
    virtual ~Person() = default;
    virtual void displayInfo() const=0;
};

#endif
