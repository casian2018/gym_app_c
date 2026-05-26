cmake_minimum_required(VERSION 3.16)
project(gym_app_c)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
set(CMAKE_CXX_STANDARD 17)

# Find optional ImGui in external/imgui. If not present, build without GUI.
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
if(EXISTS ${IMGUI_DIR}/imgui.cpp)
    add_library(imgui_lib
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl2.cpp
    )

    target_include_directories(imgui_lib PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )

    if(UNIX)
        find_package(X11 REQUIRED)
        find_package(OpenGL REQUIRED)
        target_link_libraries(imgui_lib PUBLIC X11::X11 OpenGL::GL)
    else()
        find_package(OpenGL)
        if(OpenGL_FOUND)
            target_link_libraries(imgui_lib PUBLIC OpenGL::GL)
        endif()
    endif()

    set(HAS_IMGUI TRUE)
else()
    message(STATUS "ImGui not found in ${IMGUI_DIR}; building without GUI support")
endif()

# Collect project sources so the executable links all implementations
set(SOURCES
    main.cpp
    FitnessCenter/fitness_center.cpp
    Member/member.cpp
    Person/person.cpp
    Subscription/subscription.cpp
    Trainer/trainer.cpp
)

add_executable(gym_app_c ${SOURCES})

if(HAS_IMGUI)
    target_link_libraries(gym_app_c PRIVATE imgui_lib)
endif()
