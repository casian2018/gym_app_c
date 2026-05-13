#include "Subscription.h"
#include <string>
#include <utility>

Subscription::Subscription(std::string t, float p, std::string v) {}

float Subscription::getPrice() const {
    return price;
}

std::string Subscription::getType() const {
    return type;
}

std::string Subscription::getValidation() const {
    return validUntil;
}




