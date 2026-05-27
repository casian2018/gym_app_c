#ifndef FITNESS_CENTER_H
#define FITNESS_CENTER_H
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "../Member/Member.h"
#include "../Trainer/Trainer.h"

class Fitness_Center {
public:
    struct Appointment {
        long long memberId = 0;
        long long trainerId = 0;
        std::string timeSlot;
    };

private:
    struct GymProfile {
        std::vector<std::shared_ptr<Member>> members;
        std::vector<std::shared_ptr<Trainer>> trainers;
        std::string storageFilename;
    };

    std::vector<std::shared_ptr<Member>> members;
    std::vector<std::shared_ptr<Trainer>> trainers;
    std::string gymName;
    std::string storageFilename;
    bool loadingFromFile;
    std::unordered_map<std::string, GymProfile> gymCache;
    std::vector<Appointment> appointments;

    void cacheCurrentGym();
    void restoreGymProfile(const GymProfile& profile);
    void cacheAppointments();

public:
    explicit Fitness_Center(std::string, std::string storageFile = "gym.txt");
    const std::string& getGymName() const;
    const std::string& getStorageFilename() const;
    void addMember(const Member& m);
    void addTrainer(const Trainer& t);
    void displayAll() const;
    const std::vector<std::shared_ptr<Member>>& getMembers() const;
    const std::vector<std::shared_ptr<Trainer>>& getTrainers() const;
    std::vector<std::string> snapshotLines() const;
    void saveToFile();
    void saveToFile(const std::string& filename);
    void loadFromFile();
    void loadFromFile(const std::string& filename);
    void switchGym(const std::string& newGymName);
    std::vector<std::string> getAvailableGyms() const;
    bool removeMember(long long memberId);
    bool removeTrainer(long long trainerId);
    bool updateMemberSubscription(long long memberId, const Subscription& subscription);
    void addAppointment(long long memberId, long long trainerId, const std::string& timeSlot);
    const std::vector<Appointment>& getAppointments() const;
    bool trainerIsBusy(long long trainerId) const;
    void clearTrainerAppointments(long long trainerId);

};

#endif
