#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_MEMBERS 500
#define MAX_TRAINERS 150
#define MAX_APPOINTMENTS 600
#define MAX_PAYMENTS 800

#define NAME_LEN 64
#define PHONE_LEN 24
#define EMAIL_LEN 80
#define PLAN_LEN 24
#define DATE_LEN 11
#define TIME_LEN 6
#define STATUS_LEN 20
#define METHOD_LEN 24
#define NOTE_LEN 120
#define SPECIALTY_LEN 64

#define DATA_DIR "data"
#define MEMBERS_FILE "data/members.txt"
#define TRAINERS_FILE "data/trainers.txt"
#define APPOINTMENTS_FILE "data/appointments.txt"
#define PAYMENTS_FILE "data/payments.txt"

#define C_RESET "\033[0m"
#define C_BOLD "\033[1m"
#define C_DIM "\033[2m"
#define C_RED "\033[31m"
#define C_GREEN "\033[32m"
#define C_YELLOW "\033[33m"
#define C_BLUE "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN "\033[36m"
#define C_WHITE "\033[37m"
#define BG_BLUE "\033[44m"
#define BG_GREEN "\033[42m"
#define BG_RED "\033[41m"

typedef struct {
    int id;
    char name[NAME_LEN];
    int age;
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
    char plan[PLAN_LEN];
    char joinDate[DATE_LEN];
    char endDate[DATE_LEN];
    char status[STATUS_LEN];
} Member;

typedef struct {
    int id;
    char name[NAME_LEN];
    char specialty[SPECIALTY_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
    double hourlyRate;
    char status[STATUS_LEN];
} Trainer;

typedef struct {
    int id;
    int memberId;
    int trainerId;
    char date[DATE_LEN];
    char time[TIME_LEN];
    char type[PLAN_LEN];
    char status[STATUS_LEN];
    char notes[NOTE_LEN];
} Appointment;

typedef struct {
    int id;
    int memberId;
    double amount;
    char date[DATE_LEN];
    char method[METHOD_LEN];
    char reason[NOTE_LEN];
} Payment;

static Member members[MAX_MEMBERS];
static Trainer trainers[MAX_TRAINERS];
static Appointment appointments[MAX_APPOINTMENTS];
static Payment payments[MAX_PAYMENTS];
static int memberCount = 0, trainerCount = 0, appointmentCount = 0, paymentCount = 0;
static int nextMemberId = 1, nextTrainerId = 1, nextAppointmentId = 1, nextPaymentId = 1;

void clearScreen(void) { printf("\033[2J\033[H"); }
void pauseEnter(void) { char tmp[8]; printf(C_DIM "\nPress Enter to continue..." C_RESET); fgets(tmp, sizeof(tmp), stdin); }

void trimNewline(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) s[--len] = '\0';
}

void stripPipes(char *s) {
    for (int i = 0; s[i]; i++) if (s[i] == '|') s[i] = '/';
}

void readLine(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (!fgets(buffer, (int)size, stdin)) {
        buffer[0] = '\0';
        return;
    }
    trimNewline(buffer);
    stripPipes(buffer);
}

int readInt(const char *prompt, int min, int max) {
    char line[64];
    int value;
    while (1) {
        readLine(prompt, line, sizeof(line));
        if (sscanf(line, "%d", &value) == 1 && value >= min && value <= max) return value;
        printf(C_RED "Invalid number. Please enter %d to %d.\n" C_RESET, min, max);
    }
}

double readDouble(const char *prompt, double min, double max) {
    char line[64];
    double value;
    while (1) {
        readLine(prompt, line, sizeof(line));
        if (sscanf(line, "%lf", &value) == 1 && value >= min && value <= max) return value;
        printf(C_RED "Invalid amount. Please enter %.2f to %.2f.\n" C_RESET, min, max);
    }
}

int isLeap(int y) { return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); }

int validDate(const char *date) {
    int y, m, d;
    int days[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') return 0;
    if (sscanf(date, "%4d-%2d-%2d", &y, &m, &d) != 3) return 0;
    if (y < 1900 || y > 2200 || m < 1 || m > 12) return 0;
    if (m == 2 && isLeap(y)) days[2] = 29;
    return d >= 1 && d <= days[m];
}

int validTimeText(const char *t) {
    int h, m;
    if (strlen(t) != 5 || t[2] != ':') return 0;
    if (sscanf(t, "%2d:%2d", &h, &m) != 2) return 0;
    return h >= 0 && h <= 23 && m >= 0 && m <= 59;
}

void readDate(const char *prompt, char *date) {
    while (1) {
        readLine(prompt, date, DATE_LEN);
        if (validDate(date)) return;
        printf(C_RED "Invalid date. Use YYYY-MM-DD, for example 2026-05-04.\n" C_RESET);
    }
}

void readTimeText(const char *prompt, char *timeText) {
    while (1) {
        readLine(prompt, timeText, TIME_LEN);
        if (validTimeText(timeText)) return;
        printf(C_RED "Invalid time. Use HH:MM, for example 18:30.\n" C_RESET);
    }
}

void today(char *out) {
    time_t now = time(NULL);
    struct tm *tmNow = localtime(&now);
    strftime(out, DATE_LEN, "%Y-%m-%d", tmNow);
}

int compareDate(const char *a, const char *b) { return strcmp(a, b); }

void addMonths(const char *start, int months, char *out) {
    int y, m, d;
    int days[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    sscanf(start, "%4d-%2d-%2d", &y, &m, &d);
    m += months;
    while (m > 12) { m -= 12; y++; }
    while (m < 1) { m += 12; y--; }
    if (m == 2 && isLeap(y)) days[2] = 29;
    if (d > days[m]) d = days[m];
    snprintf(out, DATE_LEN, "%04d-%02d-%02d", y, m, d);
}

int containsIgnoreCase(const char *text, const char *needle) {
    if (needle[0] == '\0') return 1;
    size_t n = strlen(needle);
    for (size_t i = 0; text[i]; i++) {
        size_t j = 0;
        while (j < n && text[i + j] && tolower((unsigned char)text[i + j]) == tolower((unsigned char)needle[j])) j++;
        if (j == n) return 1;
    }
    return 0;
}

Member *findMember(int id) {
    for (int i = 0; i < memberCount; i++) if (members[i].id == id) return &members[i];
    return NULL;
}

Trainer *findTrainer(int id) {
    for (int i = 0; i < trainerCount; i++) if (trainers[i].id == id) return &trainers[i];
    return NULL;
}

const char *memberName(int id) {
    Member *m = findMember(id);
    return m ? m->name : "Unknown member";
}

const char *trainerName(int id) {
    Trainer *t = findTrainer(id);
    return t ? t->name : "No trainer";
}

const char *statusColor(const char *status) {
    if (containsIgnoreCase(status, "active") || containsIgnoreCase(status, "paid") || containsIgnoreCase(status, "scheduled")) return C_GREEN;
    if (containsIgnoreCase(status, "expired") || containsIgnoreCase(status, "cancel")) return C_RED;
    if (containsIgnoreCase(status, "hold") || containsIgnoreCase(status, "pending")) return C_YELLOW;
    return C_WHITE;
}

void syncMemberStatuses(void) {
    char now[DATE_LEN];
    today(now);
    for (int i = 0; i < memberCount; i++) {
        if (strcmp(members[i].status, "Frozen") != 0 && compareDate(members[i].endDate, now) < 0) strcpy(members[i].status, "Expired");
        else if (strcmp(members[i].status, "Expired") == 0 && compareDate(members[i].endDate, now) >= 0) strcpy(members[i].status, "Active");
    }
}

void banner(const char *title) {
    clearScreen();
    printf(C_CYAN C_BOLD "============================================================\n" C_RESET);
    printf(C_WHITE C_BOLD "  %s\n" C_RESET, title);
    printf(C_CYAN C_BOLD "============================================================\n" C_RESET);
}

void printMemberHeader(void) {
    printf(C_BOLD "%-4s %-22s %-4s %-14s %-13s %-11s %-11s %-10s\n" C_RESET, "ID", "Name", "Age", "Phone", "Plan", "Joined", "Ends", "Status");
    printf(C_DIM "------------------------------------------------------------------------------------------------\n" C_RESET);
}

void printMember(const Member *m) {
    printf("%-4d %-22.22s %-4d %-14.14s %-13.13s %-11s %-11s %s%-10s%s\n",
           m->id, m->name, m->age, m->phone, m->plan, m->joinDate, m->endDate, statusColor(m->status), m->status, C_RESET);
}

void printTrainerHeader(void) {
    printf(C_BOLD "%-4s %-22s %-22s %-14s %-10s %-10s\n" C_RESET, "ID", "Name", "Specialty", "Phone", "Rate", "Status");
    printf(C_DIM "--------------------------------------------------------------------------------------\n" C_RESET);
}

void printTrainer(const Trainer *t) {
    printf("%-4d %-22.22s %-22.22s %-14.14s $%-9.2f %s%-10s%s\n",
           t->id, t->name, t->specialty, t->phone, t->hourlyRate, statusColor(t->status), t->status, C_RESET);
}

void printAppointmentHeader(void) {
    printf(C_BOLD "%-4s %-18s %-18s %-11s %-6s %-14s %-12s\n" C_RESET, "ID", "Member", "Trainer", "Date", "Time", "Type", "Status");
    printf(C_DIM "------------------------------------------------------------------------------------------------\n" C_RESET);
}

void printAppointment(const Appointment *a) {
    printf("%-4d %-18.18s %-18.18s %-11s %-6s %-14.14s %s%-12s%s\n",
           a->id, memberName(a->memberId), trainerName(a->trainerId), a->date, a->time, a->type, statusColor(a->status), a->status, C_RESET);
}

void saveAll(void) {
    mkdir(DATA_DIR, 0755);
    FILE *f = fopen(MEMBERS_FILE, "w");
    if (f) {
        for (int i = 0; i < memberCount; i++) fprintf(f, "%d|%s|%d|%s|%s|%s|%s|%s|%s\n", members[i].id, members[i].name, members[i].age, members[i].phone, members[i].email, members[i].plan, members[i].joinDate, members[i].endDate, members[i].status);
        fclose(f);
    }
    f = fopen(TRAINERS_FILE, "w");
    if (f) {
        for (int i = 0; i < trainerCount; i++) fprintf(f, "%d|%s|%s|%s|%s|%.2f|%s\n", trainers[i].id, trainers[i].name, trainers[i].specialty, trainers[i].phone, trainers[i].email, trainers[i].hourlyRate, trainers[i].status);
        fclose(f);
    }
    f = fopen(APPOINTMENTS_FILE, "w");
    if (f) {
        for (int i = 0; i < appointmentCount; i++) fprintf(f, "%d|%d|%d|%s|%s|%s|%s|%s\n", appointments[i].id, appointments[i].memberId, appointments[i].trainerId, appointments[i].date, appointments[i].time, appointments[i].type, appointments[i].status, appointments[i].notes);
        fclose(f);
    }
    f = fopen(PAYMENTS_FILE, "w");
    if (f) {
        for (int i = 0; i < paymentCount; i++) fprintf(f, "%d|%d|%.2f|%s|%s|%s\n", payments[i].id, payments[i].memberId, payments[i].amount, payments[i].date, payments[i].method, payments[i].reason);
        fclose(f);
    }
}

void loadMembers(void) {
    FILE *f = fopen(MEMBERS_FILE, "r");
    char line[512];
    if (!f) return;
    while (memberCount < MAX_MEMBERS && fgets(line, sizeof(line), f)) {
        trimNewline(line);
        Member m;
        char *p = strtok(line, "|"); if (!p) continue; m.id = atoi(p);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(m.name, p, NAME_LEN);
        p = strtok(NULL, "|"); if (!p) continue; m.age = atoi(p);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(m.phone, p, PHONE_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(m.email, p, EMAIL_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(m.plan, p, PLAN_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(m.joinDate, p, DATE_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(m.endDate, p, DATE_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(m.status, p, STATUS_LEN);
        m.name[NAME_LEN - 1] = m.phone[PHONE_LEN - 1] = m.email[EMAIL_LEN - 1] = m.plan[PLAN_LEN - 1] = m.joinDate[DATE_LEN - 1] = m.endDate[DATE_LEN - 1] = m.status[STATUS_LEN - 1] = '\0';
        members[memberCount++] = m;
        if (m.id >= nextMemberId) nextMemberId = m.id + 1;
    }
    fclose(f);
}

void loadTrainers(void) {
    FILE *f = fopen(TRAINERS_FILE, "r");
    char line[512];
    if (!f) return;
    while (trainerCount < MAX_TRAINERS && fgets(line, sizeof(line), f)) {
        trimNewline(line);
        Trainer t;
        char *p = strtok(line, "|"); if (!p) continue; t.id = atoi(p);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(t.name, p, NAME_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(t.specialty, p, SPECIALTY_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(t.phone, p, PHONE_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(t.email, p, EMAIL_LEN);
        p = strtok(NULL, "|"); if (!p) continue; t.hourlyRate = atof(p);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(t.status, p, STATUS_LEN);
        t.name[NAME_LEN - 1] = t.specialty[SPECIALTY_LEN - 1] = t.phone[PHONE_LEN - 1] = t.email[EMAIL_LEN - 1] = t.status[STATUS_LEN - 1] = '\0';
        trainers[trainerCount++] = t;
        if (t.id >= nextTrainerId) nextTrainerId = t.id + 1;
    }
    fclose(f);
}

void loadAppointments(void) {
    FILE *f = fopen(APPOINTMENTS_FILE, "r");
    char line[700];
    if (!f) return;
    while (appointmentCount < MAX_APPOINTMENTS && fgets(line, sizeof(line), f)) {
        trimNewline(line);
        Appointment a;
        char *p = strtok(line, "|"); if (!p) continue; a.id = atoi(p);
        p = strtok(NULL, "|"); if (!p) continue; a.memberId = atoi(p);
        p = strtok(NULL, "|"); if (!p) continue; a.trainerId = atoi(p);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(a.date, p, DATE_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(a.time, p, TIME_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(a.type, p, PLAN_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(a.status, p, STATUS_LEN);
        p = strtok(NULL, "|"); if (!p) p = ""; strncpy(a.notes, p, NOTE_LEN);
        a.date[DATE_LEN - 1] = a.time[TIME_LEN - 1] = a.type[PLAN_LEN - 1] = a.status[STATUS_LEN - 1] = a.notes[NOTE_LEN - 1] = '\0';
        appointments[appointmentCount++] = a;
        if (a.id >= nextAppointmentId) nextAppointmentId = a.id + 1;
    }
    fclose(f);
}

void loadPayments(void) {
    FILE *f = fopen(PAYMENTS_FILE, "r");
    char line[512];
    if (!f) return;
    while (paymentCount < MAX_PAYMENTS && fgets(line, sizeof(line), f)) {
        trimNewline(line);
        Payment pmt;
        char *p = strtok(line, "|"); if (!p) continue; pmt.id = atoi(p);
        p = strtok(NULL, "|"); if (!p) continue; pmt.memberId = atoi(p);
        p = strtok(NULL, "|"); if (!p) continue; pmt.amount = atof(p);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(pmt.date, p, DATE_LEN);
        p = strtok(NULL, "|"); if (!p) continue; strncpy(pmt.method, p, METHOD_LEN);
        p = strtok(NULL, "|"); if (!p) p = ""; strncpy(pmt.reason, p, NOTE_LEN);
        pmt.date[DATE_LEN - 1] = pmt.method[METHOD_LEN - 1] = pmt.reason[NOTE_LEN - 1] = '\0';
        payments[paymentCount++] = pmt;
        if (pmt.id >= nextPaymentId) nextPaymentId = pmt.id + 1;
    }
    fclose(f);
}

void loadAll(void) {
    loadMembers();
    loadTrainers();
    loadAppointments();
    loadPayments();
    syncMemberStatuses();
}

void choosePlan(char *plan, char *joinDate, char *endDate) {
    int choice;
    printf(C_CYAN "\nSubscription plans:\n" C_RESET);
    printf("1. Monthly  ($45, 1 month)\n2. Quarterly ($120, 3 months)\n3. Yearly    ($399, 12 months)\n4. Custom\n");
    choice = readInt("Choose plan: ", 1, 4);
    if (choice == 1) { strcpy(plan, "Monthly"); addMonths(joinDate, 1, endDate); }
    else if (choice == 2) { strcpy(plan, "Quarterly"); addMonths(joinDate, 3, endDate); }
    else if (choice == 3) { strcpy(plan, "Yearly"); addMonths(joinDate, 12, endDate); }
    else { readLine("Plan name: ", plan, PLAN_LEN); readDate("End date (YYYY-MM-DD): ", endDate); }
}

void addMember(void) {
    if (memberCount >= MAX_MEMBERS) { printf(C_RED "Member storage is full.\n" C_RESET); pauseEnter(); return; }
    banner("Add Member");
    Member m;
    m.id = nextMemberId++;
    readLine("Full name: ", m.name, NAME_LEN);
    m.age = readInt("Age: ", 10, 100);
    readLine("Phone: ", m.phone, PHONE_LEN);
    readLine("Email: ", m.email, EMAIL_LEN);
    readDate("Join date (YYYY-MM-DD): ", m.joinDate);
    choosePlan(m.plan, m.joinDate, m.endDate);
    strcpy(m.status, "Active");
    members[memberCount++] = m;
    saveAll();
    printf(C_GREEN "\nMember added with ID %d.\n" C_RESET, m.id);
    pauseEnter();
}

void listMembers(void) {
    banner("Members");
    if (memberCount == 0) printf(C_YELLOW "No members found.\n" C_RESET);
    else { printMemberHeader(); for (int i = 0; i < memberCount; i++) printMember(&members[i]); }
    pauseEnter();
}

void searchMembers(void) {
    char q[NAME_LEN];
    int found = 0;
    banner("Search Members");
    readLine("Search name, phone, email, plan, or status: ", q, sizeof(q));
    printMemberHeader();
    for (int i = 0; i < memberCount; i++) {
        if (containsIgnoreCase(members[i].name, q) || containsIgnoreCase(members[i].phone, q) || containsIgnoreCase(members[i].email, q) || containsIgnoreCase(members[i].plan, q) || containsIgnoreCase(members[i].status, q)) {
            printMember(&members[i]); found++;
        }
    }
    if (!found) printf(C_YELLOW "No matching members.\n" C_RESET);
    pauseEnter();
}

void filterMembers(void) {
    int choice, found = 0;
    char date[DATE_LEN], plan[PLAN_LEN];
    banner("Advanced Member Filter");
    printf("1. Active members\n2. Expired members\n3. Frozen members\n4. Plan name\n5. Expiring before date\n6. Age range\n");
    choice = readInt("Filter: ", 1, 6);
    printMemberHeader();
    if (choice == 1 || choice == 2 || choice == 3) {
        const char *target = choice == 1 ? "Active" : choice == 2 ? "Expired" : "Frozen";
        for (int i = 0; i < memberCount; i++) if (strcmp(members[i].status, target) == 0) { printMember(&members[i]); found++; }
    } else if (choice == 4) {
        readLine("Plan contains: ", plan, PLAN_LEN);
        for (int i = 0; i < memberCount; i++) if (containsIgnoreCase(members[i].plan, plan)) { printMember(&members[i]); found++; }
    } else if (choice == 5) {
        readDate("End date before (YYYY-MM-DD): ", date);
        for (int i = 0; i < memberCount; i++) if (compareDate(members[i].endDate, date) <= 0) { printMember(&members[i]); found++; }
    } else {
        int minAge = readInt("Minimum age: ", 1, 120);
        int maxAge = readInt("Maximum age: ", minAge, 120);
        for (int i = 0; i < memberCount; i++) if (members[i].age >= minAge && members[i].age <= maxAge) { printMember(&members[i]); found++; }
    }
    if (!found) printf(C_YELLOW "No members matched this filter.\n" C_RESET);
    pauseEnter();
}

void updateMember(void) {
    banner("Update Member");
    int id = readInt("Member ID: ", 1, 999999);
    Member *m = findMember(id);
    if (!m) { printf(C_RED "Member not found.\n" C_RESET); pauseEnter(); return; }
    printf(C_DIM "Leave text fields blank to keep existing values.\n" C_RESET);
    char input[EMAIL_LEN];
    printf("Current: "); printMember(m);
    readLine("New name: ", input, NAME_LEN); if (strlen(input)) strcpy(m->name, input);
    int age = readInt("New age (0 keeps existing): ", 0, 120); if (age != 0) m->age = age;
    readLine("New phone: ", input, PHONE_LEN); if (strlen(input)) strcpy(m->phone, input);
    readLine("New email: ", input, EMAIL_LEN); if (strlen(input)) strcpy(m->email, input);
    printf("1. Keep plan/dates\n2. Renew/change plan\n3. Manually change status\n");
    int choice = readInt("Choice: ", 1, 3);
    if (choice == 2) {
        readDate("New start date (YYYY-MM-DD): ", m->joinDate);
        choosePlan(m->plan, m->joinDate, m->endDate);
        strcpy(m->status, "Active");
    } else if (choice == 3) {
        printf("1. Active\n2. Expired\n3. Frozen\n4. Cancelled\n");
        int s = readInt("Status: ", 1, 4);
        strcpy(m->status, s == 1 ? "Active" : s == 2 ? "Expired" : s == 3 ? "Frozen" : "Cancelled");
    }
    saveAll();
    printf(C_GREEN "Member updated.\n" C_RESET);
    pauseEnter();
}

void deleteMember(void) {
    banner("Delete Member");
    int id = readInt("Member ID: ", 1, 999999);
    int index = -1;
    for (int i = 0; i < memberCount; i++) if (members[i].id == id) index = i;
    if (index < 0) { printf(C_RED "Member not found.\n" C_RESET); pauseEnter(); return; }
    printMember(&members[index]);
    int confirm = readInt("Delete this member? 1=yes 2=no: ", 1, 2);
    if (confirm == 1) {
        for (int i = index; i < memberCount - 1; i++) members[i] = members[i + 1];
        memberCount--;
        saveAll();
        printf(C_GREEN "Member deleted. Old appointments/payments remain as history.\n" C_RESET);
    }
    pauseEnter();
}

void addTrainer(void) {
    if (trainerCount >= MAX_TRAINERS) { printf(C_RED "Trainer storage is full.\n" C_RESET); pauseEnter(); return; }
    banner("Add Trainer");
    Trainer t;
    t.id = nextTrainerId++;
    readLine("Full name: ", t.name, NAME_LEN);
    readLine("Specialty: ", t.specialty, SPECIALTY_LEN);
    readLine("Phone: ", t.phone, PHONE_LEN);
    readLine("Email: ", t.email, EMAIL_LEN);
    t.hourlyRate = readDouble("Hourly rate: ", 0, 9999);
    strcpy(t.status, "Active");
    trainers[trainerCount++] = t;
    saveAll();
    printf(C_GREEN "Trainer added with ID %d.\n" C_RESET, t.id);
    pauseEnter();
}

void listTrainers(void) {
    banner("Trainers");
    if (trainerCount == 0) printf(C_YELLOW "No trainers found.\n" C_RESET);
    else { printTrainerHeader(); for (int i = 0; i < trainerCount; i++) printTrainer(&trainers[i]); }
    pauseEnter();
}

void updateTrainer(void) {
    banner("Update Trainer");
    int id = readInt("Trainer ID: ", 1, 999999);
    Trainer *t = findTrainer(id);
    char input[EMAIL_LEN];
    if (!t) { printf(C_RED "Trainer not found.\n" C_RESET); pauseEnter(); return; }
    printTrainer(t);
    readLine("New name: ", input, NAME_LEN); if (strlen(input)) strcpy(t->name, input);
    readLine("New specialty: ", input, SPECIALTY_LEN); if (strlen(input)) strcpy(t->specialty, input);
    readLine("New phone: ", input, PHONE_LEN); if (strlen(input)) strcpy(t->phone, input);
    readLine("New email: ", input, EMAIL_LEN); if (strlen(input)) strcpy(t->email, input);
    t->hourlyRate = readDouble("Hourly rate (enter current if unchanged): ", 0, 9999);
    printf("1. Active\n2. On leave\n3. Inactive\n");
    int s = readInt("Status: ", 1, 3);
    strcpy(t->status, s == 1 ? "Active" : s == 2 ? "On leave" : "Inactive");
    saveAll();
    printf(C_GREEN "Trainer updated.\n" C_RESET);
    pauseEnter();
}

void scheduleAppointment(void) {
    if (appointmentCount >= MAX_APPOINTMENTS) { printf(C_RED "Appointment storage is full.\n" C_RESET); pauseEnter(); return; }
    banner("Schedule Appointment");
    Appointment a;
    a.id = nextAppointmentId++;
    a.memberId = readInt("Member ID: ", 1, 999999);
    if (!findMember(a.memberId)) { printf(C_RED "Member not found.\n" C_RESET); pauseEnter(); return; }
    a.trainerId = readInt("Trainer ID (0 for none): ", 0, 999999);
    if (a.trainerId != 0 && !findTrainer(a.trainerId)) { printf(C_RED "Trainer not found.\n" C_RESET); pauseEnter(); return; }
    readDate("Date (YYYY-MM-DD): ", a.date);
    readTimeText("Time (HH:MM): ", a.time);
    for (int i = 0; i < appointmentCount; i++) {
        if (appointments[i].trainerId == a.trainerId && a.trainerId != 0 && strcmp(appointments[i].date, a.date) == 0 && strcmp(appointments[i].time, a.time) == 0 && strcmp(appointments[i].status, "Cancelled") != 0) {
            printf(C_RED "That trainer already has an appointment at this time.\n" C_RESET);
            pauseEnter(); return;
        }
    }
    readLine("Type (PT, Assessment, Yoga, etc.): ", a.type, PLAN_LEN);
    readLine("Notes: ", a.notes, NOTE_LEN);
    strcpy(a.status, "Scheduled");
    appointments[appointmentCount++] = a;
    saveAll();
    printf(C_GREEN "Appointment scheduled with ID %d.\n" C_RESET, a.id);
    pauseEnter();
}

void listAppointments(void) {
    banner("Appointments");
    if (appointmentCount == 0) printf(C_YELLOW "No appointments found.\n" C_RESET);
    else { printAppointmentHeader(); for (int i = 0; i < appointmentCount; i++) printAppointment(&appointments[i]); }
    pauseEnter();
}

void filterAppointments(void) {
    int choice, id, found = 0;
    char date[DATE_LEN], status[STATUS_LEN];
    banner("Filter Appointments");
    printf("1. By member\n2. By trainer\n3. By date\n4. By status\n5. Upcoming from today\n");
    choice = readInt("Filter: ", 1, 5);
    if (choice == 1) id = readInt("Member ID: ", 1, 999999);
    else if (choice == 2) id = readInt("Trainer ID: ", 1, 999999);
    else if (choice == 3) readDate("Date: ", date);
    else if (choice == 4) readLine("Status contains: ", status, STATUS_LEN);
    else today(date);
    printAppointmentHeader();
    for (int i = 0; i < appointmentCount; i++) {
        int ok = 0;
        if (choice == 1) ok = appointments[i].memberId == id;
        else if (choice == 2) ok = appointments[i].trainerId == id;
        else if (choice == 3) ok = strcmp(appointments[i].date, date) == 0;
        else if (choice == 4) ok = containsIgnoreCase(appointments[i].status, status);
        else ok = compareDate(appointments[i].date, date) >= 0 && strcmp(appointments[i].status, "Cancelled") != 0;
        if (ok) { printAppointment(&appointments[i]); found++; }
    }
    if (!found) printf(C_YELLOW "No appointments matched this filter.\n" C_RESET);
    pauseEnter();
}

void updateAppointmentStatus(void) {
    banner("Update Appointment Status");
    int id = readInt("Appointment ID: ", 1, 999999);
    for (int i = 0; i < appointmentCount; i++) {
        if (appointments[i].id == id) {
            printAppointment(&appointments[i]);
            printf("1. Scheduled\n2. Completed\n3. Cancelled\n4. No-show\n");
            int s = readInt("Status: ", 1, 4);
            strcpy(appointments[i].status, s == 1 ? "Scheduled" : s == 2 ? "Completed" : s == 3 ? "Cancelled" : "No-show");
            saveAll();
            printf(C_GREEN "Appointment status updated.\n" C_RESET);
            pauseEnter(); return;
        }
    }
    printf(C_RED "Appointment not found.\n" C_RESET);
    pauseEnter();
}

void addPayment(void) {
    if (paymentCount >= MAX_PAYMENTS) { printf(C_RED "Payment storage is full.\n" C_RESET); pauseEnter(); return; }
    banner("Record Payment");
    Payment p;
    p.id = nextPaymentId++;
    p.memberId = readInt("Member ID: ", 1, 999999);
    if (!findMember(p.memberId)) { printf(C_RED "Member not found.\n" C_RESET); pauseEnter(); return; }
    p.amount = readDouble("Amount: ", 0.01, 1000000);
    readDate("Payment date (YYYY-MM-DD): ", p.date);
    readLine("Method (cash/card/bank/app): ", p.method, METHOD_LEN);
    readLine("Reason: ", p.reason, NOTE_LEN);
    payments[paymentCount++] = p;
    saveAll();
    printf(C_GREEN "Payment recorded with ID %d.\n" C_RESET, p.id);
    pauseEnter();
}

void listPayments(void) {
    banner("Payments");
    if (paymentCount == 0) printf(C_YELLOW "No payments found.\n" C_RESET);
    else {
        printf(C_BOLD "%-4s %-22s %-10s %-11s %-12s %-30s\n" C_RESET, "ID", "Member", "Amount", "Date", "Method", "Reason");
        printf(C_DIM "----------------------------------------------------------------------------------------------\n" C_RESET);
        for (int i = 0; i < paymentCount; i++) printf("%-4d %-22.22s $%-9.2f %-11s %-12.12s %-30.30s\n", payments[i].id, memberName(payments[i].memberId), payments[i].amount, payments[i].date, payments[i].method, payments[i].reason);
    }
    pauseEnter();
}

void paymentReports(void) {
    char from[DATE_LEN], to[DATE_LEN];
    double total = 0;
    int count = 0;
    banner("Revenue Report");
    readDate("From date (YYYY-MM-DD): ", from);
    readDate("To date (YYYY-MM-DD): ", to);
    printf(C_BOLD "%-4s %-22s %-10s %-11s %-12s %-30s\n" C_RESET, "ID", "Member", "Amount", "Date", "Method", "Reason");
    printf(C_DIM "----------------------------------------------------------------------------------------------\n" C_RESET);
    for (int i = 0; i < paymentCount; i++) {
        if (compareDate(payments[i].date, from) >= 0 && compareDate(payments[i].date, to) <= 0) {
            printf("%-4d %-22.22s $%-9.2f %-11s %-12.12s %-30.30s\n", payments[i].id, memberName(payments[i].memberId), payments[i].amount, payments[i].date, payments[i].method, payments[i].reason);
            total += payments[i].amount; count++;
        }
    }
    printf(C_GREEN C_BOLD "\nPayments: %d | Total revenue: $%.2f\n" C_RESET, count, total);
    pauseEnter();
}

void dashboard(void) {
    int active = 0, expired = 0, frozen = 0, upcoming = 0;
    double totalRevenue = 0;
    char now[DATE_LEN]; today(now);
    syncMemberStatuses();
    for (int i = 0; i < memberCount; i++) {
        if (strcmp(members[i].status, "Active") == 0) active++;
        else if (strcmp(members[i].status, "Expired") == 0) expired++;
        else if (strcmp(members[i].status, "Frozen") == 0) frozen++;
    }
    for (int i = 0; i < appointmentCount; i++) if (compareDate(appointments[i].date, now) >= 0 && strcmp(appointments[i].status, "Cancelled") != 0) upcoming++;
    for (int i = 0; i < paymentCount; i++) totalRevenue += payments[i].amount;
    banner("Gym Dashboard");
    printf(BG_BLUE C_WHITE C_BOLD " Members " C_RESET " Total: %d | " C_GREEN "Active: %d" C_RESET " | " C_RED "Expired: %d" C_RESET " | " C_YELLOW "Frozen: %d" C_RESET "\n", memberCount, active, expired, frozen);
    printf(BG_GREEN C_WHITE C_BOLD " Staff   " C_RESET " Trainers: %d | Upcoming appointments: %d\n", trainerCount, upcoming);
    printf(BG_RED C_WHITE C_BOLD " Money   " C_RESET " Payments: %d | Lifetime revenue: " C_GREEN "$%.2f" C_RESET "\n\n", paymentCount, totalRevenue);
    printf(C_CYAN C_BOLD "Members expiring in the next 30 days\n" C_RESET);
    printMemberHeader();
    int shown = 0;
    char future[DATE_LEN]; addMonths(now, 1, future);
    for (int i = 0; i < memberCount; i++) {
        if (strcmp(members[i].status, "Active") == 0 && compareDate(members[i].endDate, now) >= 0 && compareDate(members[i].endDate, future) <= 0) {
            printMember(&members[i]); shown++;
        }
    }
    if (!shown) printf(C_DIM "No memberships expiring soon. Nice.\n" C_RESET);
    pauseEnter();
}

void seedDemoData(void) {
    banner("Demo Data");
    if (memberCount || trainerCount || appointmentCount || paymentCount) {
        printf(C_YELLOW "Demo data can only be added when the database is empty.\n" C_RESET);
        pauseEnter(); return;
    }
    Member sampleMembers[] = {
        {1,"Alex Ionescu",28,"0712345678","alex@example.com","Monthly","2026-05-01","2026-06-01","Active"},
        {2,"Maria Popescu",34,"0722222222","maria@example.com","Yearly","2026-01-15","2027-01-15","Active"},
        {3,"Dan Marin",42,"0733333333","dan@example.com","Quarterly","2026-01-01","2026-04-01","Expired"}
    };
    Trainer sampleTrainers[] = {
        {1,"Elena Dobre","Strength","0744444444","elena@gym.com",35.0,"Active"},
        {2,"Victor Stan","Yoga","0755555555","victor@gym.com",30.0,"Active"}
    };
    Appointment sampleAppointments[] = {
        {1,1,1,"2026-05-05","18:00","PT","Scheduled","Upper body session"},
        {2,2,2,"2026-05-06","09:30","Yoga","Scheduled","Mobility focus"}
    };
    Payment samplePayments[] = {
        {1,1,45.0,"2026-05-01","card","Monthly subscription"},
        {2,2,399.0,"2026-01-15","bank","Yearly subscription"}
    };
    memcpy(members, sampleMembers, sizeof(sampleMembers)); memberCount = 3; nextMemberId = 4;
    memcpy(trainers, sampleTrainers, sizeof(sampleTrainers)); trainerCount = 2; nextTrainerId = 3;
    memcpy(appointments, sampleAppointments, sizeof(sampleAppointments)); appointmentCount = 2; nextAppointmentId = 3;
    memcpy(payments, samplePayments, sizeof(samplePayments)); paymentCount = 2; nextPaymentId = 3;
    saveAll();
    printf(C_GREEN "Demo data added.\n" C_RESET);
    pauseEnter();
}

void exportReport(void) {
    FILE *f;
    char now[DATE_LEN]; today(now);
    mkdir(DATA_DIR, 0755);
    f = fopen("data/report_summary.txt", "w");
    if (!f) { printf(C_RED "Could not write report file.\n" C_RESET); pauseEnter(); return; }
    double revenue = 0;
    for (int i = 0; i < paymentCount; i++) revenue += payments[i].amount;
    fprintf(f, "GYM MANAGEMENT SYSTEM REPORT\nGenerated: %s\n\n", now);
    fprintf(f, "Members: %d\nTrainers: %d\nAppointments: %d\nPayments: %d\nRevenue: $%.2f\n\n", memberCount, trainerCount, appointmentCount, paymentCount, revenue);
    fprintf(f, "Active Members\n");
    for (int i = 0; i < memberCount; i++) if (strcmp(members[i].status, "Active") == 0) fprintf(f, "%d - %s - %s until %s\n", members[i].id, members[i].name, members[i].plan, members[i].endDate);
    fclose(f);
    printf(C_GREEN "Report exported to data/report_summary.txt\n" C_RESET);
    pauseEnter();
}

void membersMenu(void) {
    int choice;
    do {
        banner("Members Menu");
        printf("1. Add member\n2. View all members\n3. Search members\n4. Advanced filter view\n5. Update/renew member\n6. Delete member\n0. Back\n");
        choice = readInt("Choice: ", 0, 6);
        if (choice == 1) addMember();
        else if (choice == 2) listMembers();
        else if (choice == 3) searchMembers();
        else if (choice == 4) filterMembers();
        else if (choice == 5) updateMember();
        else if (choice == 6) deleteMember();
    } while (choice != 0);
}

void trainersMenu(void) {
    int choice;
    do {
        banner("Trainers Menu");
        printf("1. Add trainer\n2. View trainers\n3. Update trainer\n0. Back\n");
        choice = readInt("Choice: ", 0, 3);
        if (choice == 1) addTrainer();
        else if (choice == 2) listTrainers();
        else if (choice == 3) updateTrainer();
    } while (choice != 0);
}

void appointmentsMenu(void) {
    int choice;
    do {
        banner("Appointments Menu");
        printf("1. Schedule appointment\n2. View appointments\n3. Filter appointments\n4. Update status\n0. Back\n");
        choice = readInt("Choice: ", 0, 4);
        if (choice == 1) scheduleAppointment();
        else if (choice == 2) listAppointments();
        else if (choice == 3) filterAppointments();
        else if (choice == 4) updateAppointmentStatus();
    } while (choice != 0);
}

void paymentsMenu(void) {
    int choice;
    do {
        banner("Payments Menu");
        printf("1. Record payment\n2. View payments\n3. Revenue report by date\n0. Back\n");
        choice = readInt("Choice: ", 0, 3);
        if (choice == 1) addPayment();
        else if (choice == 2) listPayments();
        else if (choice == 3) paymentReports();
    } while (choice != 0);
}

int main(void) {
    int choice;
    loadAll();
    do {
        banner("Gym Management System");
        printf(C_GREEN "1. Dashboard and reports\n" C_RESET);
        printf(C_CYAN "2. Members\n" C_RESET);
        printf(C_MAGENTA "3. Trainers\n" C_RESET);
        printf(C_YELLOW "4. Appointments\n" C_RESET);
        printf(C_BLUE "5. Payments\n" C_RESET);
        printf("6. Export summary report\n");
        printf("7. Add demo data\n");
        printf("8. Save now\n");
        printf(C_RED "0. Exit\n" C_RESET);
        choice = readInt("Choice: ", 0, 8);
        if (choice == 1) dashboard();
        else if (choice == 2) membersMenu();
        else if (choice == 3) trainersMenu();
        else if (choice == 4) appointmentsMenu();
        else if (choice == 5) paymentsMenu();
        else if (choice == 6) exportReport();
        else if (choice == 7) seedDemoData();
        else if (choice == 8) { saveAll(); printf(C_GREEN "Saved.\n" C_RESET); pauseEnter(); }
    } while (choice != 0);
    saveAll();
    printf(C_GREEN "Data saved. Goodbye!\n" C_RESET);
    return 0;
}
