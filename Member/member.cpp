#include <iostream>
#include "member.h"
#include <utility>
#include <chrono>
#include <ctime>

using namespace std;

namespace {
/// @brief Gets current date stamp in YYYYMMDD format.
std::string currentDateStamp() {
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

long long memberSequence = 0;       ///< Counter for members added today
long long memberSequenceDate = 0;   ///< Date when sequence started

/// @brief Generates unique Member ID (date-based + sequence number).
long long generateMemberId() {
    const long long dateStamp = currentDateStamp();
    // Reset sequence if new day
    if (dateStamp != memberSequenceDate) {
        memberSequenceDate = dateStamp;
        memberSequence = 1;
    } else {
        ++memberSequence;
    }
    return dateStamp * 10000LL + memberSequence;
}

/// @brief Synchronizes ID counter when loading existing members.
void syncMemberId(long long id) {
    const long long dateStamp = id / 10000LL;
    const long long sequence = id % 10000LL;
    if (dateStamp == currentDateStamp() && sequence > memberSequence) {
        memberSequenceDate = dateStamp;
        memberSequence = sequence;
    }
}
}

/// @brief Constructor with explicit ID (used when loading from file).
Member::Member(long long id, std::string name, int age, std::string email, std::string phone, Subscription membershipType)
    : Person(std::move(name), age, std::move(email), std::move(phone)), memberId(id), sub(membershipType) {
    syncMemberId(memberId);
}

/// @brief Constructor that auto-generates unique ID.
Member::Member(std::string name, int age, std::string email, std::string phone, Subscription membershipType)
    : Person(std::move(name), age, std::move(email), std::move(phone)), memberId(generateMemberId()), sub(membershipType) {}

/// @brief Displays member profile with all details.
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

/// @brief Returns member's unique ID.
long long Member::getId() const {
    return memberId;
}

/// @brief Updates member's subscription.
void Member::setSubscription(const Subscription& membershipType) {
    sub = membershipType;
}
