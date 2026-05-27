#include <iostream>
#include "member.h"
#include <utility>
#include <chrono>
#include <ctime>

using namespace std;

namespace {
long long currentDateStamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};

#if defined(_WIN32)
    localtime_s(&localTime, &nowTime);
#else
    localtime_r(&nowTime, &localTime);
#endif

    const long long year = localTime.tm_year + 1900;
    const long long month = localTime.tm_mon + 1;
    const long long day = localTime.tm_mday;
    return year * 10000LL + month * 100LL + day;
}

long long memberSequence = 0;
long long memberSequenceDate = 0;

long long generateMemberId() {
    const long long dateStamp = currentDateStamp();
    if (dateStamp != memberSequenceDate) {
        memberSequenceDate = dateStamp;
        memberSequence = 1;
    } else {
        ++memberSequence;
    }
    return dateStamp * 10000LL + memberSequence;
}

void syncMemberId(long long id) {
    const long long dateStamp = id / 10000LL;
    const long long sequence = id % 10000LL;
    if (dateStamp == currentDateStamp() && sequence > memberSequence) {
        memberSequenceDate = dateStamp;
        memberSequence = sequence;
    }
}
}

Member::Member(long long id, std::string name, int age, std::string email, std::string phone, Subscription membershipType)
    : Person(std::move(name), age, std::move(email), std::move(phone)), memberId(id), sub(membershipType) {
    syncMemberId(memberId);
}

Member::Member(std::string name, int age, std::string email, std::string phone, Subscription membershipType)
    : Person(std::move(name), age, std::move(email), std::move(phone)), memberId(generateMemberId()), sub(membershipType) {}

void Member::displayInfo() const {
        cout << "--------------------------" << endl;
        cout << "MEMBER PROFILE" << endl;
        cout << "ID: " << memberId << endl;
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "Membership Type: " << sub.getType() << endl;
        cout << "Contact: " << email << " | " << phoneNumber << endl;
        cout << "--------------------------" << endl;
}

long long Member::getId() const {
    return memberId;
}

void Member::setSubscription(const Subscription& membershipType) {
    sub = membershipType;
}
