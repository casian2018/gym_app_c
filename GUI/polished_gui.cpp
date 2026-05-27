#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "../FitnessCenter/fitness_center.h"

namespace {
constexpr int kBtnMember = 1001;
constexpr int kBtnTrainer = 1002;
constexpr int kBtnViewData = 1003;
constexpr int kBtnAppointment = 1004;
constexpr int kBtnManage = 1005;
constexpr int kBtnExit = 1006;
constexpr int kBtnSwitchGym = 1007;
constexpr int kBtnAddGym = 1008;
constexpr int kListGyms = 1009;
constexpr int kBtnAdd = 2001;
constexpr int kBtnCloseForm = 2002;
constexpr int kBtnMembers = 3001;
constexpr int kBtnTrainers = 3002;
constexpr int kBtnDataClose = 3003;
constexpr int kBtnPromptOk = 5001;
constexpr int kBtnPromptCancel = 5002;
constexpr int kEditName = 4001;
constexpr int kEditAge = 4002;
constexpr int kEditEmail = 4003;
constexpr int kEditPhone = 4004;
constexpr int kEditExtra = 4005;
constexpr int kBtnApptBook = 6001;
constexpr int kBtnApptClose = 6002;
constexpr int kComboApptMember = 6003;
constexpr int kComboApptTrainer = 6004;
constexpr int kDateApptTime = 6005;
constexpr int kComboApptDate = 6006;
constexpr int kBtnManageMembers = 7001;
constexpr int kBtnManageTrainers = 7002;
constexpr int kBtnManageAction1 = 7003;
constexpr int kBtnManageAction2 = 7004;
constexpr int kBtnManageAction3 = 7005;
constexpr int kBtnManageClose = 7006;
constexpr int kComboSubscriptionType = 8001;
constexpr int kComboSubscriptionStatus = 8002;
constexpr int kDateSubscriptionValid = 8003;

constexpr COLORREF kHomeBg = RGB(26, 26, 31);
constexpr COLORREF kHomeCard = RGB(38, 38, 48);
constexpr COLORREF kWhite = RGB(245, 245, 245);
constexpr COLORREF kDataBg = RGB(248, 249, 251);

struct FormState {
    bool memberMode = true;
    HWND titleText = nullptr;
    HWND nameLabel = nullptr;
    HWND nameEdit = nullptr;
    HWND ageLabel = nullptr;
    HWND ageEdit = nullptr;
    HWND emailLabel = nullptr;
    HWND emailEdit = nullptr;
    HWND phoneLabel = nullptr;
    HWND phoneEdit = nullptr;
    HWND extraLabel = nullptr;
    HWND subscriptionTypeLabel = nullptr;
    HWND subscriptionStatusLabel = nullptr;
    HWND subscriptionValidLabel = nullptr;
    HWND extraEdit = nullptr;
    HWND subscriptionTypeCombo = nullptr;
    HWND subscriptionStatusCombo = nullptr;
    HWND subscriptionValidPicker = nullptr;
    HWND subscriptionCombo = nullptr;
    HWND addButton = nullptr;
    HWND closeButton = nullptr;
    HWND statusText = nullptr;
};

struct HomeState {
    HWND titleText = nullptr;
    HWND summaryText = nullptr;
    HWND switchButton = nullptr;
    HWND memberButton = nullptr;
    HWND trainerButton = nullptr;
    HWND dataButton = nullptr;
    HWND appointmentButton = nullptr;
    HWND manageButton = nullptr;
    HWND exitButton = nullptr;
};

struct SwitchState {
    HWND titleText = nullptr;
    HWND listBox = nullptr;
    HWND addButton = nullptr;
    HWND statusText = nullptr;
};

struct AppointmentState {
    HWND titleText = nullptr;
    HWND memberLabel = nullptr;
    HWND memberCombo = nullptr;
    HWND trainerLabel = nullptr;
    HWND trainerCombo = nullptr;
    HWND timePicker = nullptr;
    HWND addButton = nullptr;
    HWND closeButton = nullptr;
    HWND statusText = nullptr;
};

struct ManageState {
    bool showingMembers = true;
    HWND titleText = nullptr;
    HWND membersButton = nullptr;
    HWND trainersButton = nullptr;
    HWND listView = nullptr;
    HWND actionOne = nullptr;
    HWND actionTwo = nullptr;
    HWND actionThree = nullptr;
    HWND closeButton = nullptr;
    HWND statusText = nullptr;
};

struct DataState {
    HWND titleText = nullptr;
    HWND membersButton = nullptr;
    HWND trainersButton = nullptr;
    HWND closeButton = nullptr;
    HWND membersList = nullptr;
    HWND trainersList = nullptr;
    HWND statusText = nullptr;
    bool showingMembers = true;
};

Fitness_Center gCenter("MyGym", "gym.txt");
HINSTANCE gInstance = nullptr;
HWND gHomeWindow = nullptr;
HWND gMemberWindow = nullptr;
HWND gTrainerWindow = nullptr;
HWND gDataWindow = nullptr;
HWND gSwitchWindow = nullptr;
HWND gAppointmentWindow = nullptr;
HWND gManageWindow = nullptr;
HWND gSummaryText = nullptr;
HFONT gTitleFont = nullptr;
HFONT gBodyFont = nullptr;
HFONT gButtonFont = nullptr;
HBRUSH gHomeBrush = nullptr;
HBRUSH gDataBrush = nullptr;

struct PromptState {
    std::string label;
    std::string value;
    bool accepted = false;
    HWND edit = nullptr;
};

struct PaymentStatusState {
    std::string status;
    bool accepted = false;
};

LRESULT CALLBACK PromptProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK PaymentStatusProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool promptForText(HWND owner, const std::string &title, const std::string &label, std::string &value) {
    static bool promptClassRegistered = false;
    if (!promptClassRegistered) {
        WNDCLASSA prompt{};
        prompt.lpfnWndProc = PromptProc;
        prompt.hInstance = gInstance;
        prompt.lpszClassName = "GymAppPromptClass";
        prompt.hCursor = LoadCursor(nullptr, IDC_ARROW);
        prompt.hbrBackground = gHomeBrush;
        if (!RegisterClassA(&prompt)) {
            return false;
        }
        promptClassRegistered = true;
    }

    PromptState state{label, "", false, nullptr};
    HWND promptWindow = CreateWindowExA(
        WS_EX_DLGMODALFRAME | WS_EX_TOOLWINDOW,
        "GymAppPromptClass",
        title.c_str(),
        WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        420,
        170,
        owner,
        nullptr,
        gInstance,
        &state);

    if (!promptWindow) {
        return false;
    }

    RECT ownerRect{};
    RECT promptRect{};
    if (owner && GetWindowRect(owner, &ownerRect) && GetWindowRect(promptWindow, &promptRect)) {
        const int width = promptRect.right - promptRect.left;
        const int height = promptRect.bottom - promptRect.top;
        const int x = ownerRect.left + ((ownerRect.right - ownerRect.left) - width) / 2;
        const int y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - height) / 2;
        SetWindowPos(promptWindow, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }

    EnableWindow(owner, FALSE);

    MSG msg{};
    while (IsWindow(promptWindow) && GetMessageA(&msg, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageA(promptWindow, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    EnableWindow(owner, TRUE);
    SetForegroundWindow(owner);

    if (state.accepted) {
        value = state.value;
        return true;
    }

    return false;
}

std::string getText(HWND handle) {
    const int length = GetWindowTextLengthA(handle);
    std::string text(length, '\0');
    if (length > 0) {
        GetWindowTextA(handle, text.data(), length + 1);
    }
    return text;
}

void setText(HWND handle, const std::string &text) {
    if (handle) {
        SetWindowTextA(handle, text.c_str());
    }
}

int getInt(HWND handle) {
    try {
        const std::string text = getText(handle);
        if (text.empty()) {
            return 0;
        }
        return std::stoi(text);
    } catch (...) {
        return 0;
    }
}

void applyFont(HWND handle, HFONT font) {
    SendMessageA(handle, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
}

void moveControl(HWND handle, int x, int y, int width, int height) {
    if (handle) {
        SetWindowPos(handle, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void centerWindow(HWND hwnd, HWND owner = nullptr) {
    RECT windowRect{};
    if (!GetWindowRect(hwnd, &windowRect)) {
        return;
    }

    const int width = windowRect.right - windowRect.left;
    const int height = windowRect.bottom - windowRect.top;

    int x = 0;
    int y = 0;
    if (owner) {
        RECT ownerRect{};
        if (GetWindowRect(owner, &ownerRect)) {
            x = ownerRect.left + ((ownerRect.right - ownerRect.left) - width) / 2;
            y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - height) / 2;
        }
    } else {
        x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
        y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    }

    SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

std::string getComboSelectionText(HWND combo) {
    const int index = static_cast<int>(SendMessageA(combo, CB_GETCURSEL, 0, 0));
    if (index < 0) {
        return {};
    }

    const int length = static_cast<int>(SendMessageA(combo, CB_GETLBTEXTLEN, index, 0));
    if (length <= 0) {
        return {};
    }

    std::string text(length, '\0');
    SendMessageA(combo, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(text.data()));
    return text;
}

std::string formatSystemTime(const SYSTEMTIME& time) {
    std::ostringstream out;
    out << std::setfill('0') << std::setw(4) << time.wYear << '-'
        << std::setw(2) << time.wMonth << '-'
        << std::setw(2) << time.wDay;
    return out.str();
}

std::string dayOfWeekText(const SYSTEMTIME& time) {
    static const char* kDays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    if (time.wDayOfWeek < 7) {
        return kDays[time.wDayOfWeek];
    }
    return "Day";
}

std::string getDatePickerText(HWND picker) {
    SYSTEMTIME time{};
    if (DateTime_GetSystemtime(picker, &time) != GDT_VALID) {
        return {};
    }
    return formatSystemTime(time);
}

std::string getDatePickerWithDayText(HWND picker) {
    SYSTEMTIME time{};
    if (DateTime_GetSystemtime(picker, &time) != GDT_VALID) {
        return {};
    }
    return formatSystemTime(time) + " (" + dayOfWeekText(time) + ")";
}

void setPickerToday(HWND picker) {
    SYSTEMTIME time{};
    GetLocalTime(&time);
    DateTime_SetSystemtime(picker, GDT_VALID, &time);
}

void setPickerNextMonth(HWND picker) {
    SYSTEMTIME time{};
    GetLocalTime(&time);
    if (time.wMonth == 12) {
        time.wYear++;
        time.wMonth = 1;
    } else {
        time.wMonth++;
    }
    DateTime_SetSystemtime(picker, GDT_VALID, &time);
}

bool promptForPaymentStatus(HWND owner, std::string& result) {
    static bool statusClassRegistered = false;
    if (!statusClassRegistered) {
        WNDCLASSA statusWindow{};
        statusWindow.lpfnWndProc = PaymentStatusProc;
        statusWindow.hInstance = gInstance;
        statusWindow.lpszClassName = "GymAppPaymentStatusClass";
        statusWindow.hCursor = LoadCursor(nullptr, IDC_ARROW);
        statusWindow.hbrBackground = gHomeBrush;
        if (!RegisterClassA(&statusWindow)) {
            return false;
        }
        statusClassRegistered = true;
    }

    auto *state = new PaymentStatusState{};
    HWND statusWindow = CreateWindowA("GymAppPaymentStatusClass", "Payment Status",
                                      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                      CW_USEDEFAULT, CW_USEDEFAULT, 280, 120,
                                      owner, nullptr, gInstance, state);
    if (!statusWindow) {
        delete state;
        return false;
    }

    EnableWindow(owner, FALSE);

    MSG msg{};
    while (IsWindow(statusWindow) && GetMessageA(&msg, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageA(statusWindow, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    EnableWindow(owner, TRUE);
    SetForegroundWindow(owner);

    if (state->accepted) {
        result = state->status;
        delete state;
        return true;
    }

    delete state;
    return false;
}

void populateSubscriptionCombo(HWND combo) {
    if (!combo) {
        return;
    }

    SendMessageA(combo, CB_RESETCONTENT, 0, 0);
    SendMessageA(combo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("Basic"));
    SendMessageA(combo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("Premium"));
    SendMessageA(combo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("Student"));
    SendMessageA(combo, CB_SETCURSEL, 0, 0);
}

void populateMemberTrainerCombos(HWND memberCombo, HWND trainerCombo) {
    if (memberCombo) {
        SendMessageA(memberCombo, CB_RESETCONTENT, 0, 0);
        for (const auto& member : gCenter.getMembers()) {
            if (!member) {
                continue;
            }
            const std::string item = std::to_string(member->getId()) + " | " + member->getName();
            SendMessageA(memberCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
        }
        SendMessageA(memberCombo, CB_SETCURSEL, 0, 0);
    }

    if (trainerCombo) {
        SendMessageA(trainerCombo, CB_RESETCONTENT, 0, 0);
        for (const auto& trainer : gCenter.getTrainers()) {
            if (!trainer) {
                continue;
            }
            const std::string busy = gCenter.trainerIsBusy(trainer->getId()) ? "busy" : "free";
            const std::string item = std::to_string(trainer->getId()) + " | " + trainer->getName() + " | " + busy;
            SendMessageA(trainerCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
        }
        SendMessageA(trainerCombo, CB_SETCURSEL, 0, 0);
    }
}

long long parseIdFromComboText(const std::string& text) {
    const auto pos = text.find('|');
    if (pos == std::string::npos) {
        return 0;
    }
    try {
        return std::stoll(text.substr(0, pos));
    } catch (...) {
        return 0;
    }
}

void resizeListColumns(HWND listView, bool membersMode, int clientWidth) {
    const int availableWidth = clientWidth > 0 ? clientWidth - 48 : 800;
    const int widths[6] = {
        availableWidth * 12 / 100,
        availableWidth * 18 / 100,
        availableWidth * 10 / 100,
        membersMode ? availableWidth * 20 / 100 : availableWidth * 18 / 100,
        availableWidth * 22 / 100,
        availableWidth * 18 / 100
    };

    for (int column = 0; column < 6; ++column) {
        ListView_SetColumnWidth(listView, column, widths[column] < 70 ? 70 : widths[column]);
    }
}

std::string activeGymTitle() {
    return gCenter.getGymName();
}

void layoutHomeWindow(HWND hwnd, HomeState *state) {
    if (!state) {
        return;
    }

    RECT rect{};
    GetClientRect(hwnd, &rect);
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    const int margin = 24;
    const int contentWidth = width > margin * 2 ? std::min(width - margin * 2, 900) : width;
    const int contentLeft = (width - contentWidth) / 2;
    const int buttonGap = 12;
    const int buttonWidth = 150;
    const int buttonHeight = 38;
    const int titleHeight = 42;
    const int summaryHeight = 24;
    const int titleWidth = contentWidth;
    const int blockHeight = titleHeight + summaryHeight + 16 + buttonHeight * 3 + 24;
    const int top = std::max(24, (height - blockHeight) / 2);
    const int row1Y = top + titleHeight + summaryHeight + 16;
    const int row2Y = row1Y + buttonHeight + 12;
    const int row3Y = row2Y + buttonHeight + 12;

    moveControl(state->titleText, contentLeft, top, titleWidth, titleHeight);
    moveControl(state->summaryText, contentLeft, top + titleHeight + 8, titleWidth, summaryHeight);
    moveControl(state->switchButton, width - 156, 18, 132, 30);

    const int topRowTotal = buttonWidth * 3 + buttonGap * 2;
    int topStartX = contentLeft + (contentWidth - topRowTotal) / 2;
    if (topStartX < contentLeft) {
        topStartX = contentLeft;
    }

    const int middleRowTotal = buttonWidth * 2 + buttonGap;
    int middleStartX = contentLeft + (contentWidth - middleRowTotal) / 2;
    if (middleStartX < contentLeft) {
        middleStartX = contentLeft;
    }

    moveControl(state->memberButton, topStartX, row1Y, buttonWidth, buttonHeight);
    moveControl(state->trainerButton, topStartX + buttonWidth + buttonGap, row1Y, buttonWidth, buttonHeight);
    moveControl(state->dataButton, topStartX + (buttonWidth + buttonGap) * 2, row1Y, buttonWidth, buttonHeight);
    moveControl(state->appointmentButton, middleStartX, row2Y, buttonWidth, buttonHeight);
    moveControl(state->manageButton, middleStartX + buttonWidth + buttonGap, row2Y, buttonWidth, buttonHeight);

    const int exitStartX = contentLeft + (contentWidth - buttonWidth) / 2;
    moveControl(state->exitButton, exitStartX, row3Y, buttonWidth, buttonHeight);
}

void layoutFormWindow(HWND hwnd, FormState *state) {
    if (!state) {
        return;
    }

    RECT rect{};
    GetClientRect(hwnd, &rect);
    const int width = rect.right - rect.left;
    const int margin = 30;
    const int contentWidth = width > margin * 2 ? std::min(width - margin * 2, 620) : width;
    const int contentLeft = (width - contentWidth) / 2;
    const int left = contentLeft;
    const int labelWidth = 90;
    const int editX = left + 100;
    const int rightPadding = 30;
    const int editWidth = contentWidth - 100 - rightPadding;
    const int rowHeight = 36;

    moveControl(state->titleText, contentLeft, 12, contentWidth, 24);
    moveControl(state->nameLabel, left, 24, labelWidth, 20);
    moveControl(state->nameEdit, editX, 20, editWidth, 24);
    moveControl(state->ageLabel, left, 60, labelWidth, 20);
    moveControl(state->ageEdit, editX, 56, editWidth, 24);
    moveControl(state->emailLabel, left, 96, labelWidth, 20);
    moveControl(state->emailEdit, editX, 92, editWidth, 24);
    moveControl(state->phoneLabel, left, 132, labelWidth, 20);
    moveControl(state->phoneEdit, editX, 128, editWidth, 24);
    moveControl(state->extraLabel, left, 168, labelWidth, 20);
    if (state->memberMode) {
        moveControl(state->subscriptionTypeLabel, left, 168, labelWidth, 20);
        moveControl(state->subscriptionTypeCombo, editX, 164, editWidth, 24);
        moveControl(state->subscriptionStatusLabel, left, 206, labelWidth, 20);
        moveControl(state->subscriptionStatusCombo, editX, 202, editWidth, 24);
        moveControl(state->subscriptionValidLabel, left, 244, labelWidth, 20);
        moveControl(state->subscriptionValidPicker, editX, 240, editWidth, 24);
        moveControl(state->extraEdit, 0, 0, 0, 0);
    } else {
        moveControl(state->subscriptionTypeLabel, 0, 0, 0, 0);
        moveControl(state->subscriptionStatusLabel, 0, 0, 0, 0);
        moveControl(state->subscriptionValidLabel, 0, 0, 0, 0);
        moveControl(state->extraEdit, editX, 164, editWidth, 24);
        moveControl(state->subscriptionTypeCombo, 0, 0, 0, 0);
        moveControl(state->subscriptionStatusCombo, 0, 0, 0, 0);
        moveControl(state->subscriptionValidPicker, 0, 0, 0, 0);
    }

    const int buttonWidth = 120;
    const int buttonGap = 20;
    const int buttonsTotal = buttonWidth * 2 + buttonGap;
    int buttonStartX = (width - buttonsTotal) / 2;
    if (buttonStartX < left) {
        buttonStartX = left;
    }
    const int buttonsY = state->memberMode ? 286 : 214;
    const int statusY = state->memberMode ? 330 : 258;
    moveControl(state->addButton, buttonStartX, buttonsY, buttonWidth, 32);
    moveControl(state->closeButton, buttonStartX + buttonWidth + buttonGap, buttonsY, buttonWidth, 32);
    moveControl(state->statusText, left, statusY, width - left * 2, rowHeight);
}

void layoutDataWindow(HWND hwnd, DataState *state) {
    if (!state) {
        return;
    }

    RECT rect{};
    GetClientRect(hwnd, &rect);
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    const int margin = 24;
    const int contentWidth = width > margin * 2 ? std::min(width - margin * 2, 1040) : width;
    const int contentLeft = (width - contentWidth) / 2;
    const int listTop = 90;
    const int statusTop = height - 34;
    const int listHeight = statusTop - listTop - 8;
    const int listWidth = contentWidth;

    moveControl(state->titleText, contentLeft, 16, contentWidth, 26);

    const int buttonWidth = 110;
    const int buttonGap = 12;
    const int buttonsTotal = buttonWidth * 2 + buttonGap;
    const int buttonLeft = contentLeft + (contentWidth - buttonsTotal) / 2;
    moveControl(state->membersButton, buttonLeft, 48, buttonWidth, 28);
    moveControl(state->trainersButton, buttonLeft + buttonWidth + buttonGap, 48, buttonWidth, 28);
    moveControl(state->membersList, contentLeft, listTop, listWidth, listHeight);
    moveControl(state->trainersList, contentLeft, listTop, listWidth, listHeight);
    moveControl(state->statusText, contentLeft, statusTop, contentWidth, 20);

    resizeListColumns(state->membersList, true, listWidth);
    resizeListColumns(state->trainersList, false, listWidth);
}

void layoutSwitchWindow(HWND hwnd, SwitchState *state) {
    if (!state) {
        return;
    }

    RECT rect{};
    GetClientRect(hwnd, &rect);
    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    const int margin = 20;
    const int contentWidth = width > margin * 2 ? std::min(width - margin * 2, 520) : width;
    const int contentLeft = (width - contentWidth) / 2;

    moveControl(state->titleText, contentLeft, 16, contentWidth, 28);
    moveControl(state->listBox, contentLeft, 56, contentWidth, std::max(180, height - 140));
    moveControl(state->addButton, contentLeft, height - 72, 110, 30);
    moveControl(state->statusText, contentLeft + 124, height - 68, contentWidth - 124, 22);
}

void refreshSwitchWindow();

void refreshHomeSummary() {
    if (!gSummaryText) {
        return;
    }

    const auto &members = gCenter.getMembers();
    const auto &trainers = gCenter.getTrainers();
    std::ostringstream out;
    out << "Members: " << members.size() << "  |  Trainers: " << trainers.size();
    setText(gSummaryText, out.str());

    if (gHomeWindow) {
        auto *homeState = reinterpret_cast<HomeState*>(GetWindowLongPtrA(gHomeWindow, GWLP_USERDATA));
        if (homeState && homeState->titleText) {
            setText(homeState->titleText, activeGymTitle());
        }
        SetWindowTextA(gHomeWindow, ("Gym App - " + gCenter.getGymName()).c_str());
    }
}

void populateGymList(HWND listBox) {
    if (!listBox) {
        return;
    }

    SendMessageA(listBox, LB_RESETCONTENT, 0, 0);
    const auto gyms = gCenter.getAvailableGyms();
    int selected = -1;
    for (size_t i = 0; i < gyms.size(); ++i) {
        SendMessageA(listBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(gyms[i].c_str()));
        if (gyms[i] == gCenter.getGymName()) {
            selected = static_cast<int>(i);
        }
    }
    if (selected >= 0) {
        SendMessageA(listBox, LB_SETCURSEL, selected, 0);
    }
}

void refreshSwitchWindow() {
    if (!gSwitchWindow) {
        return;
    }

    auto *state = reinterpret_cast<SwitchState*>(GetWindowLongPtrA(gSwitchWindow, GWLP_USERDATA));
    if (!state) {
        return;
    }

    populateGymList(state->listBox);
    setText(state->statusText, "Double-click a gym to switch.");
    setText(state->titleText, "Switch Gym");
    SetWindowTextA(gSwitchWindow, ("Switch Gym - " + gCenter.getGymName()).c_str());
}

void refreshMemberList(HWND listView) {
    ListView_DeleteAllItems(listView);
    const auto &members = gCenter.getMembers();

    for (size_t index = 0; index < members.size(); ++index) {
        const auto &member = members[index];
        if (!member) {
            continue;
        }

        const std::string id = std::to_string(member->getId());
        const std::string age = std::to_string(member->getAge());
        const std::string subscription = member->getSubscriptionType();

        LVITEMA item{};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(index);
        item.iSubItem = 0;
        item.pszText = const_cast<LPSTR>(id.c_str());
        ListView_InsertItem(listView, &item);
        ListView_SetItemText(listView, static_cast<int>(index), 1, const_cast<LPSTR>(member->getName().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 2, const_cast<LPSTR>(age.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 3, const_cast<LPSTR>(subscription.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 4, const_cast<LPSTR>(member->getEmail().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 5, const_cast<LPSTR>(member->getPhoneNumber().c_str()));
    }
}

void refreshTrainerList(HWND listView) {
    ListView_DeleteAllItems(listView);
    const auto &trainers = gCenter.getTrainers();

    for (size_t index = 0; index < trainers.size(); ++index) {
        const auto &trainer = trainers[index];
        if (!trainer) {
            continue;
        }

        const std::string id = std::to_string(trainer->getId());
        const std::string age = std::to_string(trainer->getAge());

        LVITEMA item{};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(index);
        item.iSubItem = 0;
        item.pszText = const_cast<LPSTR>(id.c_str());
        ListView_InsertItem(listView, &item);
        ListView_SetItemText(listView, static_cast<int>(index), 1, const_cast<LPSTR>(trainer->getName().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 2, const_cast<LPSTR>(age.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 3, const_cast<LPSTR>(trainer->getSpecialization().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 4, const_cast<LPSTR>(trainer->getEmail().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 5, const_cast<LPSTR>(trainer->getPhoneNumber().c_str()));
    }
}

void refreshDataWindow();

void showFormStatus(HWND status, const std::string &text) {
    setText(status, text);
}

void createListColumns(HWND listView, bool membersMode) {
    ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMNA column{};
    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    if (membersMode) {
        column.cx = 110; column.pszText = const_cast<LPSTR>("ID"); ListView_InsertColumn(listView, 0, &column);
        column.cx = 160; column.pszText = const_cast<LPSTR>("Name"); ListView_InsertColumn(listView, 1, &column);
        column.cx = 70;  column.pszText = const_cast<LPSTR>("Age"); ListView_InsertColumn(listView, 2, &column);
        column.cx = 140; column.pszText = const_cast<LPSTR>("Subscription"); ListView_InsertColumn(listView, 3, &column);
        column.cx = 190; column.pszText = const_cast<LPSTR>("Email"); ListView_InsertColumn(listView, 4, &column);
        column.cx = 150; column.pszText = const_cast<LPSTR>("Phone"); ListView_InsertColumn(listView, 5, &column);
    } else {
        column.cx = 110; column.pszText = const_cast<LPSTR>("ID"); ListView_InsertColumn(listView, 0, &column);
        column.cx = 160; column.pszText = const_cast<LPSTR>("Name"); ListView_InsertColumn(listView, 1, &column);
        column.cx = 70;  column.pszText = const_cast<LPSTR>("Age"); ListView_InsertColumn(listView, 2, &column);
        column.cx = 170; column.pszText = const_cast<LPSTR>("Specialization"); ListView_InsertColumn(listView, 3, &column);
        column.cx = 190; column.pszText = const_cast<LPSTR>("Email"); ListView_InsertColumn(listView, 4, &column);
        column.cx = 150; column.pszText = const_cast<LPSTR>("Phone"); ListView_InsertColumn(listView, 5, &column);
    }
}

void clearListViewColumns(HWND listView) {
    while (ListView_DeleteColumn(listView, 0)) {
    }
}

long long getSelectedListViewId(HWND listView) {
    const int row = ListView_GetNextItem(listView, -1, LVNI_SELECTED);
    if (row < 0) {
        return 0;
    }

    char buffer[64] = {0};
    ListView_GetItemText(listView, row, 0, buffer, sizeof(buffer));
    try {
        return std::stoll(buffer);
    } catch (...) {
        return 0;
    }
}

bool confirmRemove(HWND owner, const char* question) {
    return MessageBoxA(owner, question, "Confirm", MB_ICONQUESTION | MB_OKCANCEL) == IDOK;
}

LRESULT CALLBACK HomeProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto *state = reinterpret_cast<HomeState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        gHomeWindow = hwnd;
        auto *newState = new HomeState{};
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(newState));
        newState->titleText = CreateWindowA("STATIC", activeGymTitle().c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER, 40, 24, 520, 40, hwnd, nullptr, gInstance, nullptr);
        gSummaryText = newState->summaryText = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE | SS_CENTER, 40, 72, 520, 22, hwnd, nullptr, gInstance, nullptr);
        newState->switchButton = CreateWindowA("BUTTON", "Switch Gym", WS_CHILD | WS_VISIBLE, 0, 0, 132, 30, hwnd, reinterpret_cast<HMENU>(kBtnSwitchGym), gInstance, nullptr);
        newState->memberButton = CreateWindowA("BUTTON", "Add Member", WS_CHILD | WS_VISIBLE, 60, 120, 150, 34, hwnd, reinterpret_cast<HMENU>(kBtnMember), gInstance, nullptr);
        newState->trainerButton = CreateWindowA("BUTTON", "Add Trainer", WS_CHILD | WS_VISIBLE, 220, 120, 150, 34, hwnd, reinterpret_cast<HMENU>(kBtnTrainer), gInstance, nullptr);
        newState->dataButton = CreateWindowA("BUTTON", "View Data", WS_CHILD | WS_VISIBLE, 380, 120, 150, 34, hwnd, reinterpret_cast<HMENU>(kBtnViewData), gInstance, nullptr);
        newState->appointmentButton = CreateWindowA("BUTTON", "Make Appointment", WS_CHILD | WS_VISIBLE, 60, 168, 150, 34, hwnd, reinterpret_cast<HMENU>(kBtnAppointment), gInstance, nullptr);
        newState->manageButton = CreateWindowA("BUTTON", "Manage", WS_CHILD | WS_VISIBLE, 220, 168, 150, 34, hwnd, reinterpret_cast<HMENU>(kBtnManage), gInstance, nullptr);
        newState->exitButton = CreateWindowA("BUTTON", "Exit", WS_CHILD | WS_VISIBLE, 380, 168, 150, 34, hwnd, reinterpret_cast<HMENU>(kBtnExit), gInstance, nullptr);
        applyFont(newState->titleText, gTitleFont);
        applyFont(newState->summaryText, gBodyFont);
        applyFont(newState->switchButton, gButtonFont);
        applyFont(newState->memberButton, gButtonFont);
        applyFont(newState->trainerButton, gButtonFont);
        applyFont(newState->dataButton, gButtonFont);
        applyFont(newState->appointmentButton, gButtonFont);
        applyFont(newState->manageButton, gButtonFont);
        applyFont(newState->exitButton, gButtonFont);
        layoutHomeWindow(hwnd, newState);
        refreshHomeSummary();
        centerWindow(hwnd, nullptr);
        return 0;
    }
    case WM_SIZE:
        layoutHomeWindow(hwnd, state);
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case kBtnSwitchGym: {
            if (!gSwitchWindow) {
                gSwitchWindow = CreateWindowExA(WS_EX_TOOLWINDOW, "GymAppSwitchClass", "Switch Gym", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                               CW_USEDEFAULT, CW_USEDEFAULT, 520, 420, hwnd, nullptr, gInstance, nullptr);
            } else {
                ShowWindow(gSwitchWindow, SW_SHOW);
                SetForegroundWindow(gSwitchWindow);
                refreshSwitchWindow();
            }
            return 0;
        }
        case kBtnMember: {
            if (!gMemberWindow) {
                gMemberWindow = CreateWindowExA(WS_EX_TOOLWINDOW, "GymAppFormClass", "Add Member", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                    CW_USEDEFAULT, CW_USEDEFAULT, 560, 420, hwnd, nullptr, gInstance,
                                                reinterpret_cast<LPVOID>(1));
            } else {
                ShowWindow(gMemberWindow, SW_SHOW);
                SetForegroundWindow(gMemberWindow);
            }
            return 0;
        }
        case kBtnTrainer: {
            if (!gTrainerWindow) {
                gTrainerWindow = CreateWindowExA(WS_EX_TOOLWINDOW, "GymAppFormClass", "Add Trainer", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                                 CW_USEDEFAULT, CW_USEDEFAULT, 560, 420, hwnd, nullptr, gInstance,
                                                 reinterpret_cast<LPVOID>(0));
            } else {
                ShowWindow(gTrainerWindow, SW_SHOW);
                SetForegroundWindow(gTrainerWindow);
            }
            return 0;
        }
        case kBtnViewData: {
            if (!gDataWindow) {
                gDataWindow = CreateWindowExA(WS_EX_TOOLWINDOW, "GymAppDataClass", "Gym Data", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                              CW_USEDEFAULT, CW_USEDEFAULT, 940, 620, hwnd, nullptr, gInstance, nullptr);
            } else {
                ShowWindow(gDataWindow, SW_SHOW);
                SetForegroundWindow(gDataWindow);
            }
            return 0;
        }
        case kBtnAppointment: {
            if (!gAppointmentWindow) {
                gAppointmentWindow = CreateWindowExA(WS_EX_TOOLWINDOW, "GymAppAppointmentClass", "Make Appointment", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                                     CW_USEDEFAULT, CW_USEDEFAULT, 540, 320, hwnd, nullptr, gInstance, nullptr);
            } else {
                ShowWindow(gAppointmentWindow, SW_SHOW);
                SetForegroundWindow(gAppointmentWindow);
            }
            return 0;
        }
        case kBtnManage: {
            if (!gManageWindow) {
                gManageWindow = CreateWindowExA(WS_EX_TOOLWINDOW, "GymAppManageClass", "Manage Members & Trainers", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                                CW_USEDEFAULT, CW_USEDEFAULT, 720, 520, hwnd, nullptr, gInstance, nullptr);
            } else {
                ShowWindow(gManageWindow, SW_SHOW);
                SetForegroundWindow(gManageWindow);
            }
            return 0;
        }
        case kBtnExit:
            DestroyWindow(hwnd);
            return 0;
        default:
            break;
        }
        break;
    case WM_CTLCOLORSTATIC: {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, kWhite);
        return reinterpret_cast<LRESULT>(gHomeBrush);
    }
    case WM_ERASEBKGND: {
        RECT rect{};
        GetClientRect(hwnd, &rect);
        FillRect(reinterpret_cast<HDC>(wParam), &rect, gHomeBrush);
        return 1;
    }
    case WM_DESTROY:
        if (state) {
            delete state;
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
        }
        gHomeWindow = nullptr;
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK SwitchProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto *state = reinterpret_cast<SwitchState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        auto *newState = new SwitchState{};
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(newState));
        gSwitchWindow = hwnd;

        newState->titleText = CreateWindowA("STATIC", "Switch Gym", WS_CHILD | WS_VISIBLE | SS_CENTER,
                                            20, 16, 460, 28, hwnd, nullptr, gInstance, nullptr);
        newState->listBox = CreateWindowExA(WS_EX_CLIENTEDGE, "LISTBOX", "",
                                            WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | WS_VSCROLL,
                                            20, 56, 460, 250, hwnd, reinterpret_cast<HMENU>(kListGyms), gInstance, nullptr);
        newState->addButton = CreateWindowA("BUTTON", "Add New", WS_CHILD | WS_VISIBLE,
                                            20, 320, 100, 30, hwnd, reinterpret_cast<HMENU>(kBtnAddGym), gInstance, nullptr);
        newState->statusText = CreateWindowA("STATIC", "Double-click a gym to switch.", WS_CHILD | WS_VISIBLE,
                                             130, 326, 350, 20, hwnd, nullptr, gInstance, nullptr);

        applyFont(newState->titleText, gTitleFont);
        applyFont(newState->listBox, gBodyFont);
        applyFont(newState->addButton, gButtonFont);
        applyFont(newState->statusText, gBodyFont);

        layoutSwitchWindow(hwnd, newState);
        refreshSwitchWindow();
        centerWindow(hwnd, gHomeWindow);
        return 0;
    }
    case WM_SIZE:
        layoutSwitchWindow(hwnd, state);
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case kBtnAddGym: {
            std::string newGymName;
            if (promptForText(hwnd, "Add Gym", "New gym name:", newGymName) && !newGymName.empty()) {
                gCenter.switchGym(newGymName);
                refreshHomeSummary();
                refreshDataWindow();
                refreshSwitchWindow();
            }
            return 0;
        }
        case kListGyms: {
            if (HIWORD(wParam) == LBN_DBLCLK && state && state->listBox) {
                const int index = static_cast<int>(SendMessageA(state->listBox, LB_GETCURSEL, 0, 0));
                if (index >= 0) {
                    const int len = static_cast<int>(SendMessageA(state->listBox, LB_GETTEXTLEN, index, 0));
                    if (len > 0) {
                        std::string gym(len, '\0');
                        SendMessageA(state->listBox, LB_GETTEXT, index, reinterpret_cast<LPARAM>(gym.data()));
                        if (!gym.empty() && gym != gCenter.getGymName()) {
                            gCenter.switchGym(gym);
                            refreshHomeSummary();
                            refreshDataWindow();
                            refreshSwitchWindow();
                        }
                    }
                }
            }
            return 0;
        }
        default:
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        if (state) {
            delete state;
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
        }
        gSwitchWindow = nullptr;
        return 0;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void refreshAppointmentWindow();
void refreshManageWindow();

void populateManageMemberList(HWND listView) {
    clearListViewColumns(listView);
    ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    LVCOLUMNA column{};
    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    column.cx = 110; column.pszText = const_cast<LPSTR>("ID"); ListView_InsertColumn(listView, 0, &column);
    column.cx = 170; column.pszText = const_cast<LPSTR>("Name"); ListView_InsertColumn(listView, 1, &column);
    column.cx = 140; column.pszText = const_cast<LPSTR>("Subscription"); ListView_InsertColumn(listView, 2, &column);
    column.cx = 120; column.pszText = const_cast<LPSTR>("Status"); ListView_InsertColumn(listView, 3, &column);
    column.cx = 150; column.pszText = const_cast<LPSTR>("Valid Until"); ListView_InsertColumn(listView, 4, &column);
    ListView_DeleteAllItems(listView);

    const auto& members = gCenter.getMembers();
    for (size_t index = 0; index < members.size(); ++index) {
        const auto& member = members[index];
        if (!member) {
            continue;
        }

        const std::string id = std::to_string(member->getId());
        const std::string name = member->getName();
        const std::string subscription = member->getSubscriptionType();
        const std::string status = member->getSubscription().getPaymentStatus();
        const std::string validUntil = member->getSubscription().getValidation();

        LVITEMA item{};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(index);
        item.iSubItem = 0;
        item.pszText = const_cast<LPSTR>(id.c_str());
        ListView_InsertItem(listView, &item);
        ListView_SetItemText(listView, static_cast<int>(index), 1, const_cast<LPSTR>(name.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 2, const_cast<LPSTR>(subscription.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 3, const_cast<LPSTR>(status.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 4, const_cast<LPSTR>(validUntil.c_str()));
    }
}

void populateManageTrainerList(HWND listView) {
    clearListViewColumns(listView);
    ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMNA column{};
    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    column.cx = 110; column.pszText = const_cast<LPSTR>("ID"); ListView_InsertColumn(listView, 0, &column);
    column.cx = 170; column.pszText = const_cast<LPSTR>("Name"); ListView_InsertColumn(listView, 1, &column);
    column.cx = 120; column.pszText = const_cast<LPSTR>("Busy"); ListView_InsertColumn(listView, 2, &column);
    column.cx = 200; column.pszText = const_cast<LPSTR>("Specialization"); ListView_InsertColumn(listView, 3, &column);

    ListView_DeleteAllItems(listView);
    const auto& trainers = gCenter.getTrainers();
    for (size_t index = 0; index < trainers.size(); ++index) {
        const auto& trainer = trainers[index];
        if (!trainer) {
            continue;
        }

        const std::string id = std::to_string(trainer->getId());
        const std::string name = trainer->getName();
        const std::string busy = gCenter.trainerIsBusy(trainer->getId()) ? "Busy" : "Free";
        const std::string specialization = trainer->getSpecialization();

        LVITEMA item{};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(index);
        item.iSubItem = 0;
        item.pszText = const_cast<LPSTR>(id.c_str());
        ListView_InsertItem(listView, &item);
        ListView_SetItemText(listView, static_cast<int>(index), 1, const_cast<LPSTR>(name.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 2, const_cast<LPSTR>(busy.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 3, const_cast<LPSTR>(specialization.c_str()));
    }
}

LRESULT CALLBACK AppointmentProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto *state = reinterpret_cast<AppointmentState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        auto *newState = new AppointmentState{};
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(newState));
        gAppointmentWindow = hwnd;

        newState->titleText = CreateWindowA("STATIC", "Make Appointment", WS_CHILD | WS_VISIBLE | SS_CENTER,
                                            20, 16, 480, 28, hwnd, nullptr, gInstance, nullptr);
        newState->memberLabel = CreateWindowA("STATIC", "Member:", WS_CHILD | WS_VISIBLE, 20, 58, 90, 20, hwnd, nullptr, gInstance, nullptr);
        newState->memberCombo = CreateWindowExA(WS_EX_CLIENTEDGE, "COMBOBOX", "",
                                                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                                                120, 54, 360, 160, hwnd, reinterpret_cast<HMENU>(kComboApptMember), gInstance, nullptr);
        newState->trainerLabel = CreateWindowA("STATIC", "Trainer:", WS_CHILD | WS_VISIBLE, 20, 96, 90, 20, hwnd, nullptr, gInstance, nullptr);
        newState->trainerCombo = CreateWindowExA(WS_EX_CLIENTEDGE, "COMBOBOX", "",
                                                 WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                                                 120, 92, 360, 160, hwnd, reinterpret_cast<HMENU>(kComboApptTrainer), gInstance, nullptr);
        newState->timePicker = CreateWindowExA(WS_EX_CLIENTEDGE, DATETIMEPICK_CLASSA, "",
                                                                                             WS_CHILD | WS_VISIBLE | DTS_SHORTDATEFORMAT,
                                                                                             120, 134, 240, 24, hwnd, reinterpret_cast<HMENU>(kDateApptTime), gInstance, nullptr);
        newState->addButton = CreateWindowA("BUTTON", "Book", WS_CHILD | WS_VISIBLE,
                                                                                        120, 176, 110, 30, hwnd, reinterpret_cast<HMENU>(kBtnApptBook), gInstance, nullptr);
        newState->closeButton = CreateWindowA("BUTTON", "Close", WS_CHILD | WS_VISIBLE,
                                                                                            240, 176, 110, 30, hwnd, reinterpret_cast<HMENU>(kBtnApptClose), gInstance, nullptr);
                newState->statusText = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 20, 220, 460, 20, hwnd, nullptr, gInstance, nullptr);

        applyFont(newState->titleText, gTitleFont);
        applyFont(newState->memberLabel, gBodyFont);
        applyFont(newState->memberCombo, gBodyFont);
        applyFont(newState->trainerLabel, gBodyFont);
        applyFont(newState->trainerCombo, gBodyFont);
        applyFont(newState->timePicker, gBodyFont);
        applyFont(newState->addButton, gButtonFont);
        applyFont(newState->closeButton, gButtonFont);
        applyFont(newState->statusText, gBodyFont);

        populateMemberTrainerCombos(newState->memberCombo, newState->trainerCombo);
        setPickerToday(newState->timePicker);
        centerWindow(hwnd, gHomeWindow);
        return 0;
    }
    case WM_NOTIFY: {
        auto* nmhdr = reinterpret_cast<NMHDR*>(lParam);
        if (nmhdr && nmhdr->idFrom == kDateApptTime && nmhdr->code == DTN_DATETIMECHANGE) {
            auto* dateState = reinterpret_cast<AppointmentState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));
            (void)dateState;
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case kBtnApptBook: {
            if (!state) {
                return 0;
            }
            const std::string memberText = getComboSelectionText(state->memberCombo);
            const std::string trainerText = getComboSelectionText(state->trainerCombo);
            const std::string timeSlot = getDatePickerWithDayText(state->timePicker);
            const long long memberId = parseIdFromComboText(memberText);
            const long long trainerId = parseIdFromComboText(trainerText);
            if (memberId == 0 || trainerId == 0 || timeSlot.empty()) {
                setText(state->statusText, "Pick a member, a trainer, and a date.");
                return 0;
            }

            gCenter.addAppointment(memberId, trainerId, timeSlot);
            setText(state->statusText, "Appointment saved automatically.");
            refreshManageWindow();
            populateMemberTrainerCombos(state->memberCombo, state->trainerCombo);
            setPickerToday(state->timePicker);
            return 0;
        }
        case kBtnApptClose:
            DestroyWindow(hwnd);
            return 0;
        default:
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        if (state) {
            delete state;
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
        }
        gAppointmentWindow = nullptr;
        return 0;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void refreshManageWindow() {
    if (!gManageWindow) {
        return;
    }

    auto *state = reinterpret_cast<ManageState*>(GetWindowLongPtrA(gManageWindow, GWLP_USERDATA));
    if (!state) {
        return;
    }

    if (state->showingMembers) {
        setText(state->titleText, "Manage Members");
        setText(state->membersButton, "Members");
        setText(state->trainersButton, "Trainers");
        setText(state->actionOne, "Remove Member");
        setText(state->actionTwo, "Cancel Sub");
        setText(state->actionThree, "Mark Paid / Late");
        populateManageMemberList(state->listView);
        setText(state->statusText, "Select a member and use an action button.");
    } else {
        setText(state->titleText, "Manage Trainers");
        setText(state->membersButton, "Members");
        setText(state->trainersButton, "Trainers");
        setText(state->actionOne, "Remove Trainer");
        setText(state->actionTwo, "Clear Schedule");
        setText(state->actionThree, "Refresh");
        populateManageTrainerList(state->listView);
        setText(state->statusText, "Select a trainer and use an action button.");
    }

    SetWindowTextA(gManageWindow, ("Manage - " + gCenter.getGymName()).c_str());
}

LRESULT CALLBACK ManageProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto *state = reinterpret_cast<ManageState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        auto *newState = new ManageState{};
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(newState));
        gManageWindow = hwnd;

        newState->titleText = CreateWindowA("STATIC", "Manage Members", WS_CHILD | WS_VISIBLE | SS_CENTER,
                                            20, 16, 660, 28, hwnd, nullptr, gInstance, nullptr);
        newState->membersButton = CreateWindowA("BUTTON", "Members", WS_CHILD | WS_VISIBLE,
                                                20, 52, 90, 28, hwnd, reinterpret_cast<HMENU>(kBtnManageMembers), gInstance, nullptr);
        newState->trainersButton = CreateWindowA("BUTTON", "Trainers", WS_CHILD | WS_VISIBLE,
                                                 120, 52, 90, 28, hwnd, reinterpret_cast<HMENU>(kBtnManageTrainers), gInstance, nullptr);
        newState->listView = CreateWindowExA(WS_EX_CLIENTEDGE, WC_LISTVIEWA, "",
                                             WS_CHILD | WS_VISIBLE | LVS_REPORT,
                                             20, 92, 660, 320, hwnd, nullptr, gInstance, nullptr);
        newState->actionOne = CreateWindowA("BUTTON", "Remove Member", WS_CHILD | WS_VISIBLE,
                                            20, 430, 150, 30, hwnd, reinterpret_cast<HMENU>(kBtnManageAction1), gInstance, nullptr);
        newState->actionTwo = CreateWindowA("BUTTON", "Cancel Sub", WS_CHILD | WS_VISIBLE,
                                            180, 430, 150, 30, hwnd, reinterpret_cast<HMENU>(kBtnManageAction2), gInstance, nullptr);
        newState->actionThree = CreateWindowA("BUTTON", "Mark Paid / Late", WS_CHILD | WS_VISIBLE,
                                              340, 430, 150, 30, hwnd, reinterpret_cast<HMENU>(kBtnManageAction3), gInstance, nullptr);
        newState->closeButton = CreateWindowA("BUTTON", "Close", WS_CHILD | WS_VISIBLE,
                                              500, 430, 150, 30, hwnd, reinterpret_cast<HMENU>(kBtnManageClose), gInstance, nullptr);
        newState->statusText = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 20, 470, 660, 20, hwnd, nullptr, gInstance, nullptr);

        applyFont(newState->titleText, gTitleFont);
        applyFont(newState->membersButton, gButtonFont);
        applyFont(newState->trainersButton, gButtonFont);
        applyFont(newState->listView, gBodyFont);
        applyFont(newState->actionOne, gButtonFont);
        applyFont(newState->actionTwo, gButtonFont);
        applyFont(newState->actionThree, gButtonFont);
        applyFont(newState->closeButton, gButtonFont);
        applyFont(newState->statusText, gBodyFont);

        refreshManageWindow();
        centerWindow(hwnd, gHomeWindow);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case kBtnManageMembers:
            if (state) {
                state->showingMembers = true;
                refreshManageWindow();
            }
            return 0;
        case kBtnManageTrainers:
            if (state) {
                state->showingMembers = false;
                refreshManageWindow();
            }
            return 0;
        case kBtnManageAction1: {
            if (!state) {
                return 0;
            }
            if (state->showingMembers) {
                const long long memberId = getSelectedListViewId(state->listView);
                if (memberId != 0 && confirmRemove(hwnd, "Remove selected member?") ) {
                    gCenter.removeMember(memberId);
                    refreshManageWindow();
                    refreshHomeSummary();
                    refreshDataWindow();
                }
            } else {
                const long long trainerId = getSelectedListViewId(state->listView);
                if (trainerId != 0 && confirmRemove(hwnd, "Remove selected trainer?") ) {
                    gCenter.removeTrainer(trainerId);
                    refreshManageWindow();
                    refreshHomeSummary();
                    refreshDataWindow();
                }
            }
            return 0;
        }
        case kBtnManageAction2: {
            if (!state) {
                return 0;
            }
            if (state->showingMembers) {
                const long long memberId = getSelectedListViewId(state->listView);
                if (memberId != 0) {
                    Subscription subscription("Cancelled", "N/A");
                    subscription.setPaymentStatus("Cancelled");
                    gCenter.updateMemberSubscription(memberId, subscription);
                    refreshManageWindow();
                    refreshDataWindow();
                }
            } else {
                const long long trainerId = getSelectedListViewId(state->listView);
                if (trainerId != 0) {
                    gCenter.clearTrainerAppointments(trainerId);
                    refreshManageWindow();
                    refreshDataWindow();
                }
            }
            return 0;
        }
        case kBtnManageAction3: {
            if (!state) {
                return 0;
            }
            if (state->showingMembers) {
                const long long memberId = getSelectedListViewId(state->listView);
                if (memberId != 0) {
                    std::string paymentStatus;
                    if (promptForPaymentStatus(hwnd, paymentStatus)) {
                        auto members = gCenter.getMembers();
                        for (const auto& member : members) {
                            if (member && member->getId() == memberId) {
                                Subscription subscription = member->getSubscription();
                                subscription.setPaymentStatus(paymentStatus);
                                gCenter.updateMemberSubscription(memberId, subscription);
                                break;
                            }
                        }
                        refreshManageWindow();
                    }
                }
            } else {
                refreshManageWindow();
            }
            return 0;
        }
        case kBtnManageClose:
            DestroyWindow(hwnd);
            return 0;
        default:
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        if (state) {
            delete state;
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
        }
        gManageWindow = nullptr;
        return 0;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK PromptProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto *state = reinterpret_cast<PromptState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        auto *createStruct = reinterpret_cast<CREATESTRUCTA*>(lParam);
        auto *newState = reinterpret_cast<PromptState*>(createStruct->lpCreateParams);
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(newState));

        CreateWindowA("STATIC", newState->label.c_str(), WS_CHILD | WS_VISIBLE, 20, 20, 360, 20, hwnd, nullptr, gInstance, nullptr);
        newState->edit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 20, 46, 360, 24, hwnd, nullptr, gInstance, nullptr);
        HWND okButton = CreateWindowA("BUTTON", "OK", WS_CHILD | WS_VISIBLE, 120, 84, 80, 28, hwnd, reinterpret_cast<HMENU>(kBtnPromptOk), gInstance, nullptr);
        HWND cancelButton = CreateWindowA("BUTTON", "Cancel", WS_CHILD | WS_VISIBLE, 210, 84, 80, 28, hwnd, reinterpret_cast<HMENU>(kBtnPromptCancel), gInstance, nullptr);
        applyFont(newState->edit, gBodyFont);
        applyFont(okButton, gButtonFont);
        applyFont(cancelButton, gButtonFont);
        SetFocus(newState->edit);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case kBtnPromptOk:
            if (state && state->edit) {
                state->value = getText(state->edit);
                state->accepted = !state->value.empty();
            }
            DestroyWindow(hwnd);
            return 0;
        case kBtnPromptCancel:
            if (state) {
                state->accepted = false;
            }
            DestroyWindow(hwnd);
            return 0;
        default:
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
        return 0;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK PaymentStatusProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto *state = reinterpret_cast<PaymentStatusState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        auto *createStruct = reinterpret_cast<CREATESTRUCTA*>(lParam);
        auto *newState = reinterpret_cast<PaymentStatusState*>(createStruct->lpCreateParams);
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(newState));

        CreateWindowA("STATIC", "Select payment status:", WS_CHILD | WS_VISIBLE, 20, 16, 240, 20, hwnd, nullptr, gInstance, nullptr);
        HWND paidButton = CreateWindowA("BUTTON", "Paid", WS_CHILD | WS_VISIBLE, 60, 50, 80, 32, hwnd, reinterpret_cast<HMENU>(7050), gInstance, nullptr);
        HWND lateButton = CreateWindowA("BUTTON", "Late", WS_CHILD | WS_VISIBLE, 160, 50, 80, 32, hwnd, reinterpret_cast<HMENU>(7051), gInstance, nullptr);
        applyFont(paidButton, gButtonFont);
        applyFont(lateButton, gButtonFont);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 7050:
            if (state) {
                state->status = "Paid";
                state->accepted = true;
            }
            DestroyWindow(hwnd);
            return 0;
        case 7051:
            if (state) {
                state->status = "Late";
                state->accepted = true;
            }
            DestroyWindow(hwnd);
            return 0;
        default:
            break;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
        return 0;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void populateFormControls(HWND hwnd, FormState *state, bool memberMode) {
    const char *title = memberMode ? "Add a member" : "Add a trainer";
    const char *extraLabel = memberMode ? "Subscription" : "Specialization";
    state->titleText = CreateWindowA("STATIC", title, WS_CHILD | WS_VISIBLE | SS_CENTER, 30, 12, 480, 24, hwnd, nullptr, gInstance, nullptr);
    state->nameLabel = CreateWindowA("STATIC", "Name:", WS_CHILD | WS_VISIBLE, 30, 24, 90, 20, hwnd, nullptr, gInstance, nullptr);
    state->nameEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 130, 20, 360, 24, hwnd,
                                    reinterpret_cast<HMENU>(kEditName), gInstance, nullptr);

    state->ageLabel = CreateWindowA("STATIC", "Age:", WS_CHILD | WS_VISIBLE, 30, 60, 90, 20, hwnd, nullptr, gInstance, nullptr);
    state->ageEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 130, 56, 360, 24, hwnd,
                                   reinterpret_cast<HMENU>(kEditAge), gInstance, nullptr);

    state->emailLabel = CreateWindowA("STATIC", "Email:", WS_CHILD | WS_VISIBLE, 30, 96, 90, 20, hwnd, nullptr, gInstance, nullptr);
    state->emailEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 130, 92, 360, 24, hwnd,
                                     reinterpret_cast<HMENU>(kEditEmail), gInstance, nullptr);

    state->phoneLabel = CreateWindowA("STATIC", "Phone:", WS_CHILD | WS_VISIBLE, 30, 132, 90, 20, hwnd, nullptr, gInstance, nullptr);
    state->phoneEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 130, 128, 360, 24, hwnd,
                                     reinterpret_cast<HMENU>(kEditPhone), gInstance, nullptr);

    state->extraLabel = CreateWindowA("STATIC", extraLabel, WS_CHILD | WS_VISIBLE, 30, 168, 90, 20, hwnd, nullptr, gInstance, nullptr);
    if (memberMode) {
        state->subscriptionTypeLabel = CreateWindowA("STATIC", "Type:", WS_CHILD | WS_VISIBLE, 30, 206, 90, 20, hwnd, nullptr, gInstance, nullptr);
        state->subscriptionTypeCombo = CreateWindowExA(0, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                                                       130, 164, 360, 120, hwnd, reinterpret_cast<HMENU>(kComboSubscriptionType), gInstance, nullptr);
        state->subscriptionStatusLabel = CreateWindowA("STATIC", "Status:", WS_CHILD | WS_VISIBLE, 30, 244, 90, 20, hwnd, nullptr, gInstance, nullptr);
        state->subscriptionStatusCombo = CreateWindowExA(0, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                                                        130, 202, 360, 120, hwnd, reinterpret_cast<HMENU>(kComboSubscriptionStatus), gInstance, nullptr);
        state->subscriptionValidLabel = CreateWindowA("STATIC", "Valid until:", WS_CHILD | WS_VISIBLE, 30, 282, 90, 20, hwnd, nullptr, gInstance, nullptr);
        state->subscriptionValidPicker = CreateWindowExA(WS_EX_CLIENTEDGE, DATETIMEPICK_CLASSA, "",
                                                        WS_CHILD | WS_VISIBLE | DTS_SHORTDATEFORMAT,
                                                        130, 240, 220, 24, hwnd, reinterpret_cast<HMENU>(kDateSubscriptionValid), gInstance, nullptr);
        populateSubscriptionCombo(state->subscriptionTypeCombo);
        SendMessageA(state->subscriptionStatusCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("Paid"));
        SendMessageA(state->subscriptionStatusCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("Late"));
        SendMessageA(state->subscriptionStatusCombo, CB_SETCURSEL, 0, 0);
        setPickerNextMonth(state->subscriptionValidPicker);
    } else {
        state->extraEdit = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 130, 164, 360, 24, hwnd,
                                         reinterpret_cast<HMENU>(kEditExtra), gInstance, nullptr);
    }

    state->addButton = CreateWindowA("BUTTON", "Add", WS_CHILD | WS_VISIBLE, 130, 208, 120, 32, hwnd, reinterpret_cast<HMENU>(kBtnAdd), gInstance, nullptr);
    state->closeButton = CreateWindowA("BUTTON", "Close", WS_CHILD | WS_VISIBLE, 270, 208, 120, 32, hwnd, reinterpret_cast<HMENU>(kBtnCloseForm), gInstance, nullptr);

    state->statusText = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 30, 248, 460, 22, hwnd, nullptr, gInstance, nullptr);
    applyFont(state->titleText, gTitleFont);
    applyFont(state->nameLabel, gBodyFont);
    applyFont(state->ageLabel, gBodyFont);
    applyFont(state->emailLabel, gBodyFont);
    applyFont(state->phoneLabel, gBodyFont);
    applyFont(state->extraLabel, gBodyFont);
    applyFont(state->nameEdit, gBodyFont);
    applyFont(state->ageEdit, gBodyFont);
    applyFont(state->emailEdit, gBodyFont);
    applyFont(state->phoneEdit, gBodyFont);
    if (state->subscriptionTypeLabel) {
        applyFont(state->subscriptionTypeLabel, gBodyFont);
    }
    if (state->subscriptionStatusLabel) {
        applyFont(state->subscriptionStatusLabel, gBodyFont);
    }
    if (state->subscriptionValidLabel) {
        applyFont(state->subscriptionValidLabel, gBodyFont);
    }
    if (state->subscriptionTypeCombo) {
        applyFont(state->subscriptionTypeCombo, gBodyFont);
    }
    if (state->subscriptionStatusCombo) {
        applyFont(state->subscriptionStatusCombo, gBodyFont);
    }
    if (state->subscriptionValidPicker) {
        applyFont(state->subscriptionValidPicker, gBodyFont);
    }
    if (state->extraEdit) {
        applyFont(state->extraEdit, gBodyFont);
    }
    applyFont(state->addButton, gButtonFont);
    applyFont(state->closeButton, gButtonFont);
    applyFont(state->statusText, gBodyFont);
    layoutFormWindow(hwnd, state);
}

void handleFormAdd(HWND hwnd, FormState *state) {
    const std::string name = getText(state->nameEdit);
    const std::string email = getText(state->emailEdit);
    const std::string phone = getText(state->phoneEdit);
    const int age = getInt(state->ageEdit);
    std::string extra;

    if (state->memberMode) {
        const std::string subType = getComboSelectionText(state->subscriptionTypeCombo);
        const std::string paymentStatus = getComboSelectionText(state->subscriptionStatusCombo);
        const std::string validUntil = getDatePickerText(state->subscriptionValidPicker);
        extra = subType;

        if (subType.empty() || paymentStatus.empty() || validUntil.empty()) {
            showFormStatus(state->statusText, "Select subscription, payment status, and date.");
            return;
        }

        Subscription sub(subType, validUntil);
        sub.setPaymentStatus(paymentStatus);
        Member member(name, age, email, phone, sub);
        gCenter.addMember(member);
        showFormStatus(state->statusText, "Member saved automatically.");

        refreshHomeSummary();
        refreshDataWindow();
        setText(state->nameEdit, "");
        setText(state->ageEdit, "");
        setText(state->emailEdit, "");
        setText(state->phoneEdit, "");
        populateSubscriptionCombo(state->subscriptionTypeCombo);
        SendMessageA(state->subscriptionStatusCombo, CB_SETCURSEL, 0, 0);
        setPickerToday(state->subscriptionValidPicker);
        return;
    } else {
        extra = getText(state->extraEdit);
    }

    if (name.empty() || email.empty() || phone.empty() || extra.empty() || age <= 0) {
        showFormStatus(state->statusText, "Please fill all the fields with valid data.");
        return;
    }

    if (!state->memberMode) {
        Trainer trainer(name, age, email, phone, extra);
        gCenter.addTrainer(trainer);
        showFormStatus(state->statusText, "Trainer saved automatically.");
    }

    refreshHomeSummary();
    refreshDataWindow();
    setText(state->nameEdit, "");
    setText(state->ageEdit, "");
    setText(state->emailEdit, "");
    setText(state->phoneEdit, "");
    setText(state->extraEdit, "");
}

LRESULT CALLBACK FormProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto *state = reinterpret_cast<FormState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        auto *newState = new FormState{};
        auto *createStruct = reinterpret_cast<CREATESTRUCTA*>(lParam);
        newState->memberMode = reinterpret_cast<LPARAM>(createStruct->lpCreateParams) != 0;
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(newState));
        populateFormControls(hwnd, newState, newState->memberMode);
        centerWindow(hwnd, gHomeWindow);
        return 0;
    }
    case WM_SIZE:
        layoutFormWindow(hwnd, state);
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case kBtnAdd:
            if (state) {
                handleFormAdd(hwnd, state);
            }
            return 0;
        case kBtnCloseForm:
            DestroyWindow(hwnd);
            return 0;
        default:
            break;
        }
        break;
    case WM_CTLCOLORSTATIC: {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, kWhite);
        return reinterpret_cast<LRESULT>(gHomeBrush);
    }
    case WM_ERASEBKGND: {
        RECT rect{};
        GetClientRect(hwnd, &rect);
        FillRect(reinterpret_cast<HDC>(wParam), &rect, gHomeBrush);
        return 1;
    }
    case WM_DESTROY:
        if (state) {
            if (state->memberMode) {
                gMemberWindow = nullptr;
            } else {
                gTrainerWindow = nullptr;
            }
            delete state;
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
        }
        return 0;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void insertListColumns(HWND listView, bool membersMode) {
    ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    LVCOLUMNA column{};
    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    if (membersMode) {
        column.cx = 120; column.pszText = const_cast<LPSTR>("ID"); ListView_InsertColumn(listView, 0, &column);
        column.cx = 180; column.pszText = const_cast<LPSTR>("Name"); ListView_InsertColumn(listView, 1, &column);
        column.cx = 80;  column.pszText = const_cast<LPSTR>("Age"); ListView_InsertColumn(listView, 2, &column);
        column.cx = 160; column.pszText = const_cast<LPSTR>("Subscription"); ListView_InsertColumn(listView, 3, &column);
        column.cx = 220; column.pszText = const_cast<LPSTR>("Email"); ListView_InsertColumn(listView, 4, &column);
        column.cx = 160; column.pszText = const_cast<LPSTR>("Phone"); ListView_InsertColumn(listView, 5, &column);
    } else {
        column.cx = 120; column.pszText = const_cast<LPSTR>("ID"); ListView_InsertColumn(listView, 0, &column);
        column.cx = 180; column.pszText = const_cast<LPSTR>("Name"); ListView_InsertColumn(listView, 1, &column);
        column.cx = 80;  column.pszText = const_cast<LPSTR>("Age"); ListView_InsertColumn(listView, 2, &column);
        column.cx = 180; column.pszText = const_cast<LPSTR>("Specialization"); ListView_InsertColumn(listView, 3, &column);
        column.cx = 220; column.pszText = const_cast<LPSTR>("Email"); ListView_InsertColumn(listView, 4, &column);
        column.cx = 160; column.pszText = const_cast<LPSTR>("Phone"); ListView_InsertColumn(listView, 5, &column);
    }
}

void populateMemberData(HWND listView) {
    ListView_DeleteAllItems(listView);
    const auto &members = gCenter.getMembers();

    for (size_t index = 0; index < members.size(); ++index) {
        const auto &member = members[index];
        if (!member) {
            continue;
        }

        const std::string id = std::to_string(member->getId());
        const std::string age = std::to_string(member->getAge());
        const std::string subscription = member->getSubscriptionType();

        LVITEMA item{};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(index);
        item.iSubItem = 0;
        item.pszText = const_cast<LPSTR>(id.c_str());
        ListView_InsertItem(listView, &item);
        ListView_SetItemText(listView, static_cast<int>(index), 1, const_cast<LPSTR>(member->getName().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 2, const_cast<LPSTR>(age.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 3, const_cast<LPSTR>(subscription.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 4, const_cast<LPSTR>(member->getEmail().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 5, const_cast<LPSTR>(member->getPhoneNumber().c_str()));
    }
}

void populateTrainerData(HWND listView) {
    ListView_DeleteAllItems(listView);
    const auto &trainers = gCenter.getTrainers();

    for (size_t index = 0; index < trainers.size(); ++index) {
        const auto &trainer = trainers[index];
        if (!trainer) {
            continue;
        }

        const std::string id = std::to_string(trainer->getId());
        const std::string age = std::to_string(trainer->getAge());

        LVITEMA item{};
        item.mask = LVIF_TEXT;
        item.iItem = static_cast<int>(index);
        item.iSubItem = 0;
        item.pszText = const_cast<LPSTR>(id.c_str());
        ListView_InsertItem(listView, &item);
        ListView_SetItemText(listView, static_cast<int>(index), 1, const_cast<LPSTR>(trainer->getName().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 2, const_cast<LPSTR>(age.c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 3, const_cast<LPSTR>(trainer->getSpecialization().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 4, const_cast<LPSTR>(trainer->getEmail().c_str()));
        ListView_SetItemText(listView, static_cast<int>(index), 5, const_cast<LPSTR>(trainer->getPhoneNumber().c_str()));
    }
}

void refreshDataWindow() {
    if (!gDataWindow) {
        return;
    }

    SetWindowTextA(gDataWindow, (gCenter.getGymName() + " - Gym Data").c_str());
    auto *state = reinterpret_cast<DataState*>(GetWindowLongPtrA(gDataWindow, GWLP_USERDATA));
    if (!state) {
        return;
    }

    if (state->showingMembers) {
        ShowWindow(state->membersList, SW_SHOW);
        ShowWindow(state->trainersList, SW_HIDE);
        populateMemberData(state->membersList);
        setText(state->statusText, "Members table.");
    } else {
        ShowWindow(state->membersList, SW_HIDE);
        ShowWindow(state->trainersList, SW_SHOW);
        populateTrainerData(state->trainersList);
        setText(state->statusText, "Trainers table.");
    }
}

LRESULT CALLBACK DataProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto *state = reinterpret_cast<DataState*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        auto *newState = new DataState{};
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(newState));
        gDataWindow = hwnd;

        newState->titleText = CreateWindowA("STATIC", "Data Explorer", WS_CHILD | WS_VISIBLE | SS_CENTER, 24, 16, 860, 26, hwnd, nullptr, gInstance, nullptr);
        newState->membersButton = CreateWindowA("BUTTON", "Members", WS_CHILD | WS_VISIBLE, 24, 48, 110, 28, hwnd, reinterpret_cast<HMENU>(kBtnMembers), gInstance, nullptr);
        newState->trainersButton = CreateWindowA("BUTTON", "Trainers", WS_CHILD | WS_VISIBLE, 144, 48, 110, 28, hwnd, reinterpret_cast<HMENU>(kBtnTrainers), gInstance, nullptr);
        newState->closeButton = CreateWindowA("BUTTON", "Close", WS_CHILD | WS_VISIBLE, 800, 48, 104, 28, hwnd, reinterpret_cast<HMENU>(kBtnDataClose), gInstance, nullptr);

        newState->membersList = CreateWindowExA(WS_EX_CLIENTEDGE, WC_LISTVIEWA, "", WS_CHILD | WS_VISIBLE | LVS_REPORT,
                                                24, 90, 880, 430, hwnd, nullptr, gInstance, nullptr);
        newState->trainersList = CreateWindowExA(WS_EX_CLIENTEDGE, WC_LISTVIEWA, "", WS_CHILD | WS_VISIBLE | LVS_REPORT,
                                                 24, 90, 880, 430, hwnd, nullptr, gInstance, nullptr);
        newState->statusText = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE, 24, 530, 880, 20, hwnd, nullptr, gInstance, nullptr);

        insertListColumns(newState->membersList, true);
        insertListColumns(newState->trainersList, false);
        applyFont(newState->titleText, gTitleFont);
        applyFont(newState->membersButton, gButtonFont);
        applyFont(newState->trainersButton, gButtonFont);
        applyFont(newState->closeButton, gButtonFont);
        applyFont(newState->statusText, gBodyFont);
        layoutDataWindow(hwnd, newState);
        refreshDataWindow();
        centerWindow(hwnd, gHomeWindow);
        return 0;
    }
    case WM_SIZE:
        layoutDataWindow(hwnd, state);
        refreshDataWindow();
        return 0;
    case WM_CTLCOLORSTATIC: {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, kWhite);
        return reinterpret_cast<LRESULT>(gHomeBrush);
    }
    case WM_ERASEBKGND: {
        RECT rect{};
        GetClientRect(hwnd, &rect);
        FillRect(reinterpret_cast<HDC>(wParam), &rect, gHomeBrush);
        return 1;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case kBtnMembers:
            if (state) {
                state->showingMembers = true;
                refreshDataWindow();
            }
            return 0;
        case kBtnTrainers:
            if (state) {
                state->showingMembers = false;
                refreshDataWindow();
            }
            return 0;
        case kBtnDataClose:
            DestroyWindow(hwnd);
            return 0;
        default:
            break;
        }
        break;
    case WM_DESTROY:
        if (state) {
            delete state;
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, 0);
        }
        gDataWindow = nullptr;
        return 0;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void registerClasses() {
    WNDCLASSA home{};
    home.lpfnWndProc = HomeProc;
    home.hInstance = gInstance;
    home.lpszClassName = "GymAppHomeClass";
    home.hCursor = LoadCursor(nullptr, IDC_ARROW);
    home.hbrBackground = gHomeBrush;
    RegisterClassA(&home);

    WNDCLASSA form{};
    form.lpfnWndProc = FormProc;
    form.hInstance = gInstance;
    form.lpszClassName = "GymAppFormClass";
    form.hCursor = LoadCursor(nullptr, IDC_ARROW);
    form.hbrBackground = gHomeBrush;
    RegisterClassA(&form);

    WNDCLASSA data{};
    data.lpfnWndProc = DataProc;
    data.hInstance = gInstance;
    data.lpszClassName = "GymAppDataClass";
    data.hCursor = LoadCursor(nullptr, IDC_ARROW);
    data.hbrBackground = gHomeBrush;
    RegisterClassA(&data);

    WNDCLASSA appointment{};
    appointment.lpfnWndProc = AppointmentProc;
    appointment.hInstance = gInstance;
    appointment.lpszClassName = "GymAppAppointmentClass";
    appointment.hCursor = LoadCursor(nullptr, IDC_ARROW);
    appointment.hbrBackground = gHomeBrush;
    RegisterClassA(&appointment);

    WNDCLASSA manage{};
    manage.lpfnWndProc = ManageProc;
    manage.hInstance = gInstance;
    manage.lpszClassName = "GymAppManageClass";
    manage.hCursor = LoadCursor(nullptr, IDC_ARROW);
    manage.hbrBackground = gHomeBrush;
    RegisterClassA(&manage);

    WNDCLASSA sw{};
    sw.lpfnWndProc = SwitchProc;
    sw.hInstance = gInstance;
    sw.lpszClassName = "GymAppSwitchClass";
    sw.hCursor = LoadCursor(nullptr, IDC_ARROW);
    sw.hbrBackground = gHomeBrush;
    RegisterClassA(&sw);
}

void createFonts() {
    gTitleFont = CreateFontA(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             VARIABLE_PITCH, "Segoe UI");
    gBodyFont = CreateFontA(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                            VARIABLE_PITCH, "Segoe UI");
    gButtonFont = CreateFontA(18, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              VARIABLE_PITCH, "Segoe UI");
}

void createBrushes() {
    gHomeBrush = CreateSolidBrush(kHomeBg);
    gDataBrush = CreateSolidBrush(kDataBg);
}

void cleanupResources() {
    if (gTitleFont) DeleteObject(gTitleFont);
    if (gBodyFont) DeleteObject(gBodyFont);
    if (gButtonFont) DeleteObject(gButtonFont);
    if (gHomeBrush) DeleteObject(gHomeBrush);
    if (gDataBrush) DeleteObject(gDataBrush);
}

} // namespace

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int showCmd) {
    gInstance = instance;

    INITCOMMONCONTROLSEX controls{};
    controls.dwSize = sizeof(controls);
    controls.dwICC = ICC_LISTVIEW_CLASSES | ICC_DATE_CLASSES;
    InitCommonControlsEx(&controls);

    createBrushes();
    createFonts();
    registerClasses();

    HWND hwnd = CreateWindowExA(
        0,
        "GymAppHomeClass",
        "Gym App - Welcome",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        620,
        300,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (!hwnd) {
        cleanupResources();
        return 1;
    }

    gHomeWindow = hwnd;
    centerWindow(hwnd, nullptr);
    ShowWindow(hwnd, showCmd);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageA(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    cleanupResources();
    return static_cast<int>(msg.wParam);
}
