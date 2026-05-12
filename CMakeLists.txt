cmake_minimum_required(VERSION 4.0)
project(gym_app_c)

set(CMAKE_CXX_STANDARD 14)

include_directories(FitnessCenter)
include_directories(Member)
include_directories(Person)
include_directories(Subscription)
include_directories(Trainer)

add_executable(gym_app_c
        FitnessCenter/fitness_center.cpp
        FitnessCenter/fitness_center.h
        Member/member.h
        Member/merber.cpp
        Person/person.cpp
        Person/person.h
        Subscription/subscription.cpp
        Subscription/subscription.h
        Trainer/trainer.cpp
        Trainer/trainer.h
        main.cpp)
