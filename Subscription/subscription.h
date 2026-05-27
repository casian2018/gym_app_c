#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <string>

class Subscription {
protected:
   std::string type;
   float price;
   std::string validUntil;
   std::string paymentStatus;

public:
   Subscription(std::string t, std::string v);

   std::string getType() const;
   float getPrice() const;
   std::string getValidation() const;
   std::string getPaymentStatus() const;
   void setType(const std::string& t);
   void setPrice(float newPrice);
   void setValidation(const std::string& v);
   void setPaymentStatus(const std::string& status);
};

#endif