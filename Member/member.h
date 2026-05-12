#ifndef MEMBER_H
#define MEMBER_H

#include <string>

class Member {
protected:
  int memberId;

public:
   Member(int id, std::string name, std::string email, std::string phone, std::string membershipType);
    void displayInfo() const override;
};

#endif