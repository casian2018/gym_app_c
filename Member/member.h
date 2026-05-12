#ifndef MEMBER_H
#define MEMBER_H

#include <string>

class Member {
protected:
  int memberId;

public:
   Subscription(std::string t, float p, std::string v);
};

#endif