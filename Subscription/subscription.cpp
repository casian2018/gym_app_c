#include "Subscription.h"
#include <string>
#include <utility>
#include <iostream>

Subscription::Subscription(std::string t,std::string v) : type(std::move(t)), validUntil(std::move(v)){
    paymentStatus = "Paid";
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

float Subscription::getPrice() const {
    return price;
}

std::string Subscription::getType() const {
    return type;
}

std::string Subscription::getValidation() const {
    return validUntil;
}

std::string Subscription::getPaymentStatus() const {
    return paymentStatus;
}

void Subscription::setType(const std::string& t) {
    type = t;
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

void Subscription::setPrice(float newPrice) {
    price = newPrice;
}

void Subscription::setValidation(const std::string& v) {
    validUntil = v;
}

void Subscription::setPaymentStatus(const std::string& status) {
    paymentStatus = status;
}




