cmake_minimum_required(VERSION 3.16)
project(gym_app_c)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES
    FitnessCenter/fitness_center.cpp
    Member/member.cpp
    Person/person.cpp
    Subscription/subscription.cpp
    Trainer/trainer.cpp
)

add_executable(gym_app_c
    main.cpp
    ${CORE_SOURCES}
)

if(WIN32)
    add_executable(gym_app_gui WIN32
        GUI/polished_gui.cpp
        ${CORE_SOURCES}
    )

    target_link_libraries(gym_app_gui PRIVATE user32 gdi32 comctl32)
endif()
