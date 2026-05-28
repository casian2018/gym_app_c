    #ifndef FITNESS_CENTER_H
#define FITNESS_CENTER_H
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "../Member/Member.h"
#include "../Trainer/Trainer.h"

/// @class Fitness_Center
/// @brief Class managing an entire gym facility.
/// 
/// Stores members, trainers, appointments, and manages
/// file saving/loading. Allows switching between different gyms,
/// each with its own storage file.
class Fitness_Center {
public:
    /// @struct Appointment
    /// @brief Represents a training appointment between a member and trainer.
    struct Appointment {
        long long memberId = 0;    ///< Member's ID
        long long trainerId = 0;   ///< Trainer's ID
        std::string timeSlot;      ///< Time slot (e.g., "10:00-11:00")
    };

private:
    /// @struct GymProfile
    /// @brief Saved profile of a gym (for caching).
    struct GymProfile {
        std::vector<std::shared_ptr<Member>> members;      ///< List of members
        std::vector<std::shared_ptr<Trainer>> trainers;    ///< List of trainers
        std::string storageFilename;                       ///< Storage file
    };

    std::vector<std::shared_ptr<Member>> members;          ///< List of current gym members
    std::vector<std::shared_ptr<Trainer>> trainers;        ///< List of current gym trainers
    std::string gymName;                                   ///< Current gym name
    std::string storageFilename;                           ///< Storage file for current gym
    bool loadingFromFile;                                  ///< Flag to prevent saving while loading
    std::unordered_map<std::string, GymProfile> gymCache;  ///< Cache with profiles of other gyms
    std::vector<Appointment> appointments;                 ///< List of training appointments

    /// @brief Caches current gym (saves to gymCache).
    void cacheCurrentGym();
    
    /// @brief Restores a gym from cache.
    void restoreGymProfile(const GymProfile& profile);
    
    /// @brief Helper for caching appointments.
    void cacheAppointments();

public:
    /// @brief Constructor - creates a gym with name and storage file.
    /// @param n - Gym name
    /// @param storageFile - Storage file name (default: "gym.txt")
    explicit Fitness_Center(std::string, std::string storageFile = "gym.txt");
    
    /// @brief Returns current gym name.
    const std::string& getGymName() const;
    
    /// @brief Returns current gym storage file.
    const std::string& getStorageFilename() const;
    
    /// @brief Adds a new member to gym.
    void addMember(const Member& m);
    
    /// @brief Adds a new trainer to gym.
    void addTrainer(const Trainer& t);
    
    /// @brief Displays all members and trainers in current gym.
    void displayAll() const;
    
    /// @brief Returns list of all members in current gym.
    const std::vector<std::shared_ptr<Member>>& getMembers() const;
    
    /// @brief Returns list of all trainers in current gym.
    const std::vector<std::shared_ptr<Trainer>>& getTrainers() const;
    
    /// @brief Returns saved lines in file format.
    std::vector<std::string> snapshotLines() const;
    
    /// @brief Saves current gym to default file.
    void saveToFile();
    
    /// @brief Saves current gym to specified file.
    void saveToFile(const std::string& filename);
    
    /// @brief Loads current gym from default file.
    void loadFromFile();
    
    /// @brief Loads gym from specified file.
    void loadFromFile(const std::string& filename);
    
    /// @brief Switches to another gym (creates if not exists).
    void switchGym(const std::string& newGymName);
    
    /// @brief Returns list of all available gym names (from cache).
    std::vector<std::string> getAvailableGyms() const;
    
    /// @brief Saves list of available gyms.
    void saveGymList();
    
    /// @brief Loads list of available gyms.
    void loadGymList();
    
    /// @brief Removes a member by ID.
    bool removeMember(long long memberId);
    
    /// @brief Removes a trainer by ID.
    bool removeTrainer(long long trainerId);
    
    /// @brief Updates member's subscription.
    bool updateMemberSubscription(long long memberId, const Subscription& subscription);
    
    /// @brief Adds an appointment between member and trainer.
    void addAppointment(long long memberId, long long trainerId, const std::string& timeSlot);
    
    /// @brief Returns list of all appointments.
    const std::vector<Appointment>& getAppointments() const;
    
    /// @brief Checks if trainer already has an appointment.
    bool trainerIsBusy(long long trainerId) const;
    
    /// @brief Clears all appointments for a trainer.
    void clearTrainerAppointments(long long trainerId);

};

#endif
