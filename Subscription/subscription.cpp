#include "Subscription.h"
#include <string>
#include <utility>
#include <iostream>

/// @brief Constructor - creates subscription with type and expiration date.
/// Automatically sets price based on subscription type (Basic=120, Premium=200, Student=90).
Subscription::Subscription(std::string t,std::string v) : type(std::move(t)), validUntil(std::move(v)){
    paymentStatus = "Paid";
    // Set price based on subscription type
    if (t == "Basic") {
        price = 120.0f;
    }
    else if (t=="Premium") {
        price = 200.0f;
    }
    else if (type == "Student") {
        price = 90.0f;
    }
    else {
        price = 0.0f;
    }
}

/// @brief Returns subscription price.
float Subscription::getPrice() const {
    return price;
}

/// @brief Returns subscription type string.
std::string Subscription::getType() const {
    return type;
}

/// @brief Returns expiration/validation date.
std::string Subscription::getValidation() const {
    return validUntil;
}

/// @brief Returns payment status (Paid, Pending, Cancelled, etc).
std::string Subscription::getPaymentStatus() const {
    return paymentStatus;
}

/// @brief Changes subscription type and updates price accordingly.
void Subscription::setType(const std::string& t) {
    type = t;
    // Update price based on new type
    if (t == "Basic") {
        price = 120.0f;
    } else if (t == "Premium") {
        price = 200.0f;
    } else if (t == "Student") {
        price = 90.0f;
    } else {
        price = 0.0f;
    }
}

/// @brief Changes subscription price.
void Subscription::setPrice(float newPrice) {
    price = newPrice;
}

/// @brief Changes expiration/validation date.
void Subscription::setValidation(const std::string& v) {
    validUntil = v;
}

/// @brief Changes payment status.
void Subscription::setPaymentStatus(const std::string& status) {
    paymentStatus = status;
}




