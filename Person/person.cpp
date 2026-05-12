#include "Person.h"
#include <string>
#include <utility>

Person::Person(std::string n,int a,std::string e,std::string p) : name(std::move(n)), age(a), email(std::move(e)), phoneNumber(std::move(p)) {}

