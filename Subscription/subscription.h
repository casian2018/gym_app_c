#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <string>

/// @class Subscription
/// @brief Class representing a gym subscription.
/// 
/// Stores subscription type (Basic, Premium, Student), price,
/// expiration date, and payment status.
class Subscription {
protected:
   /// @brief Subscription type (Basic, Premium, Student).
   std::string type;
   
   /// @brief Monthly price of subscription.
   float price;
   
   /// @brief Expiration date of subscription.
   std::string validUntil;
   
   /// @brief Payment status (Paid, Pending, Cancelled).
   std::string paymentStatus;

public:
   /// @brief Constructor with type and expiration date.
   /// @param t - Subscription type
   /// @param v - Validation/expiration date
   Subscription(std::string t, std::string v);

   /// @brief Returns subscription type.
   std::string getType() const;
   
   /// @brief Returns subscription price.
   float getPrice() const;
   
   /// @brief Returns validation date.
   std::string getValidation() const;
   
   /// @brief Returns payment status.
   std::string getPaymentStatus() const;
   
   /// @brief Changes subscription type.
   void setType(const std::string& t);
   
   /// @brief Changes subscription price.
   void setPrice(float newPrice);
   
   /// @brief Changes expiration date.
   void setValidation(const std::string& v);
   
   /// @brief Changes payment status.
   void setPaymentStatus(const std::string& status);
};

#endif