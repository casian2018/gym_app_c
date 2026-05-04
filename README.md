# Gym Management System in C

A terminal-based fitness center management app written in pure C. It manages members, subscriptions, trainers, appointments, payments, reports, filtering, searching, colored menus, and file persistence.

## Compile

```bash
gcc -std=c99 -Wall -Wextra -pedantic n.c -o gym_app
```

## Run

```bash
./gym_app
```

## Features

- Colored terminal interface with dashboard cards
- Add, view, search, filter, update, renew, and delete members
- Subscription plans with automatic end dates
- Automatic expired membership detection based on today's date
- Trainer management with specialty, contact, rate, and status
- Appointment scheduling with conflict detection for trainers
- Appointment filters by member, trainer, date, status, and upcoming sessions
- Payment recording and revenue reports by date range
- Summary report export to `data/report_summary.txt`
- Persistent storage in plain text files under `data/`
- Demo data option for quick testing

## Data Files

The program automatically creates these files when saving:

- `data/members.txt`
- `data/trainers.txt`
- `data/appointments.txt`
- `data/payments.txt`
