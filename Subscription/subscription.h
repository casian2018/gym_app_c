#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <string>

class Subscription {
protected:
   std::string type;
   float price;
   std::string validUntil;

public:
   Subscription(std::string t, float p, std::string v);

   std::string getType() const;
   float getPrice() const;
   std::string getValidation() const;
};

#endif