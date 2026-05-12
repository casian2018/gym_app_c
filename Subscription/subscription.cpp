#include "subscription.h"

subscription::subscription()
    : type(""), price(0.0f), validUntil("")
{
}

subscription::subscription(const std::string& type, float price, const std::string& validUntil)
    : type(type), price(price), validUntil(validUntil)
{
}

const std::string& subscription::getType() const
{
    return type;
}

float subscription::getPrice() const
{
    return price;
}

const std::string& subscription::getValidUntil() const
{
    return validUntil;
}

void subscription::setType(const std::string& type)
{
    this->type = type;
}

void subscription::setPrice(float price)
{
    this->price = price;
}

void subscription::setValidUntil(const std::string& validUntil)
{
    this->validUntil = validUntil;
}
