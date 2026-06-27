# Gym App C

Gym App C is a C++ fitness center management application built to handle the everyday operations of a gym in a simple, modular way. It manages members, trainers, subscriptions, and appointments, while persisting data to local text files. The project ships with both a console interface and a native Windows GUI.

## What It Does

Gym App C focuses on the core workflows needed to run a small fitness center:

* **Member management:** add, list, remove, and update member data.
* **Trainer management:** add, list, and remove trainers, including their specializations.
* **Subscription handling:** support for predefined subscription types, automatic pricing, validity dates, and payment status.
* **Appointment booking:** create member-trainer appointments and check trainer availability.
* **Persistent storage:** save and load all data from plain text files automatically.
* **Multi-gym support:** switch between multiple gym profiles, each backed by its own storage file.
* **Console workflow:** a menu-driven CLI for quick management tasks.
* **Windows GUI:** native Win32 windows for adding data, browsing records, managing entities, and booking appointments.

## Tech Stack

* **Language:** C++17
* **Build system:** CMake
* **Console UI:** standard C++ I/O
* **Desktop UI:** Win32 API
* **Core libraries:** STL containers and utilities such as `vector`, `unordered_map`, `shared_ptr`, `algorithm`, `chrono`, and `sstream`

## Local Setup

### Requirements

* A C++ compiler with C++17 support
* CMake
* On Windows, the GUI build requires the standard Win32 libraries available on the system

### Build

Configure and build the project with CMake:

```bash
cmake -S . -B build
cmake --build build
```

This produces the console application `gym_app_c`. On Windows, the same project also builds the GUI target `gym_app_gui`.

### Run

Use the generated executable from your build folder. The console app accepts a storage file path as an optional argument if you want to load a specific gym profile.

## Data Files

The application stores its data in plain text files:

* `gym.txt` - current gym data
* `gyms_list.txt` - list of available gym profiles
* separate gym files generated from the gym name

## Project Structure

* `main.cpp` - console entry point with the menu-based workflow
* `FitnessCenter/` - central business logic for members, trainers, subscriptions, and appointments
* `Member/` - member model and ID generation
* `Trainer/` - trainer model and ID generation
* `Person/` - shared base class for people in the system
* `Subscription/` - subscription model and pricing logic
* `GUI/` - native Windows GUI implementation

## Design Summary

The codebase is organized around a simple domain model and a clear separation between business logic and presentation. That makes the project easy to extend, easy to inspect, and suitable for demonstrating practical C++ application design, file persistence, and desktop UI development.