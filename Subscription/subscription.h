#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <string>

class subscription
{
private:
    std::string type;
    float price;
    std::string validUntil;

public:
    subscription();
    subscription(const std::string& type, float price, const std::string& validUntil);

    const std::string& getType() const;
    float getPrice() const;
    const std::string& getValidUntil() const;

    void setType(const std::string& type);
    void setPrice(float price);
    void setValidUntil(const std::string& validUntil);
};

#endif
