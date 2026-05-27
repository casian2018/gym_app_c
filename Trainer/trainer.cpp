#include "Trainer.h"
#include <string>
#include <utility>
#include <iostream>
#include <chrono>
#include <ctime>

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

long long trainerSequence = 0;
long long trainerSequenceDate = 0;

long long generateTrainerId() {
        const long long dateStamp = currentDateStamp();
        if (dateStamp != trainerSequenceDate) {
                trainerSequenceDate = dateStamp;
                trainerSequence = 1;
        } else {
                ++trainerSequence;
        }
        return dateStamp * 10000LL + trainerSequence;
}

void syncTrainerId(long long id) {
        const long long dateStamp = id / 10000LL;
        const long long sequence = id % 10000LL;
        if (dateStamp == currentDateStamp() && sequence > trainerSequence) {
                trainerSequenceDate = dateStamp;
                trainerSequence = sequence;
        }
}
}

Trainer::Trainer(long long id, std::string n, int a, std::string e, std::string p, std::string s)
        : Person(std::move(n), a, std::move(e), std::move(p)), specialization(std::move(s)), trainerId(id) {
        syncTrainerId(trainerId);
}

Trainer::Trainer(std::string n, int a, std::string e, std::string p, std::string s)
        : Person(std::move(n), a, std::move(e), std::move(p)), specialization(std::move(s)), trainerId(generateTrainerId()) {}

void Trainer::displayInfo() const {
                std::cout << "--------------------------" << std::endl;
                std::cout << "TRAINER PROFILE" << std::endl;
                std::cout << "ID: " << trainerId << std::endl;
                std::cout << "Name: " << name << std::endl;
                std::cout << "Age: " << age << std::endl;
                std::cout << "Specialization: " << specialization << std::endl;
                std::cout << "Contact: " << email << " | " << phoneNumber << std::endl;
                std::cout << "--------------------------" << std::endl;
}
