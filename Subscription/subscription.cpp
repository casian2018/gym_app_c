#include "Subscription.h"
#include <string>
#include <utility>
#include <iostream>

Subscription::Subscription(std::string t,std::string v) : type(std::move(t)), validUntil(std::move(v)){
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
        std::cout<<"Invalid Subscription Type";
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




