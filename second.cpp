// main.cpp
// Purpose: Native pop-up visual app for a Gym Management System dashboard.
// Author: Your Name

#include <X11/Xlib.h>

#include <cstring>
#include <iostream>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        std::cerr << "Could not open a graphical window. Make sure you are running inside a desktop session.\n";
        return 1;
    }

    const int screen = DefaultScreen(display);
    const unsigned long white = WhitePixel(display, screen);
    const unsigned long black = BlackPixel(display, screen);
    const int width = 900;
    const int height = 620;

    Window window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        120,
        80,
        width,
        height,
        1,
        black,
        white
    );

    XStoreName(display, window, "Gym Management System");
    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);

    Atom deleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteMessage, 1);

    GC gc = XCreateGC(display, window, 0, nullptr);
    Colormap colorMap = DefaultColormap(display, screen);

    XColor backgroundColor;
    XColor headerColor;
    XColor cardColor;
    XColor accentGreen;
    XColor accentBlue;
    XColor accentOrange;
    XColor textColor;
    XColor mutedColor;
    XColor whiteColor;

    XParseColor(display, colorMap, "#f4f7fb", &backgroundColor);
    XAllocColor(display, colorMap, &backgroundColor);
    XParseColor(display, colorMap, "#172033", &headerColor);
    XAllocColor(display, colorMap, &headerColor);
    XParseColor(display, colorMap, "#ffffff", &cardColor);
    XAllocColor(display, colorMap, &cardColor);
    XParseColor(display, colorMap, "#19a974", &accentGreen);
    XAllocColor(display, colorMap, &accentGreen);
    XParseColor(display, colorMap, "#2f80ed", &accentBlue);
    XAllocColor(display, colorMap, &accentBlue);
    XParseColor(display, colorMap, "#f2994a", &accentOrange);
    XAllocColor(display, colorMap, &accentOrange);
    XParseColor(display, colorMap, "#1f2937", &textColor);
    XAllocColor(display, colorMap, &textColor);
    XParseColor(display, colorMap, "#64748b", &mutedColor);
    XAllocColor(display, colorMap, &mutedColor);
    XParseColor(display, colorMap, "#ffffff", &whiteColor);
    XAllocColor(display, colorMap, &whiteColor);

    XMapWindow(display, window);

    bool running = true;
    while (running) {
        XEvent event;
        XNextEvent(display, &event);

        if (event.type == ClientMessage && static_cast<Atom>(event.xclient.data.l[0]) == deleteMessage) {
            running = false;
        }

        if (event.type == KeyPress || event.type == ButtonPress) {
            running = false;
        }

        if (event.type == Expose && event.xexpose.count == 0) {
            XSetForeground(display, gc, backgroundColor.pixel);
            XFillRectangle(display, window, gc, 0, 0, width, height);

            XSetForeground(display, gc, headerColor.pixel);
            XFillRectangle(display, window, gc, 0, 0, width, 110);

            XSetForeground(display, gc, whiteColor.pixel);
            XSetFont(display, gc, XLoadFont(display, "fixed"));
            const char* title = "GYM MANAGEMENT SYSTEM";
            const char* subtitle = "Visual dashboard mockup";
            XDrawString(display, window, gc, 36, 44, title, std::strlen(title));
            XDrawString(display, window, gc, 36, 72, subtitle, std::strlen(subtitle));

            XSetForeground(display, gc, accentGreen.pixel);
            XFillRectangle(display, window, gc, 700, 32, 150, 42);
            XSetForeground(display, gc, whiteColor.pixel);
            const char* status = "OPEN";
            XDrawString(display, window, gc, 758, 58, status, std::strlen(status));

            XSetForeground(display, gc, cardColor.pixel);
            XFillRectangle(display, window, gc, 36, 140, 250, 105);
            XFillRectangle(display, window, gc, 324, 140, 250, 105);
            XFillRectangle(display, window, gc, 612, 140, 250, 105);
            XFillRectangle(display, window, gc, 36, 275, 250, 105);
            XFillRectangle(display, window, gc, 324, 275, 250, 105);
            XFillRectangle(display, window, gc, 612, 275, 250, 105);

            XSetForeground(display, gc, accentGreen.pixel);
            XFillRectangle(display, window, gc, 36, 140, 8, 105);
            XSetForeground(display, gc, accentBlue.pixel);
            XFillRectangle(display, window, gc, 324, 140, 8, 105);
            XSetForeground(display, gc, accentOrange.pixel);
            XFillRectangle(display, window, gc, 612, 140, 8, 105);
            XSetForeground(display, gc, accentBlue.pixel);
            XFillRectangle(display, window, gc, 36, 275, 8, 105);
            XSetForeground(display, gc, accentGreen.pixel);
            XFillRectangle(display, window, gc, 324, 275, 8, 105);
            XSetForeground(display, gc, accentOrange.pixel);
            XFillRectangle(display, window, gc, 612, 275, 8, 105);

            XSetForeground(display, gc, mutedColor.pixel);
            const char* members = "MEMBERS";
            const char* trainers = "TRAINERS";
            const char* appointments = "APPOINTMENTS";
            const char* subscriptions = "SUBSCRIPTIONS";
            const char* payments = "PAYMENTS";
            const char* reports = "REPORTS";
            XDrawString(display, window, gc, 62, 170, members, std::strlen(members));
            XDrawString(display, window, gc, 350, 170, trainers, std::strlen(trainers));
            XDrawString(display, window, gc, 638, 170, appointments, std::strlen(appointments));
            XDrawString(display, window, gc, 62, 305, subscriptions, std::strlen(subscriptions));
            XDrawString(display, window, gc, 350, 305, payments, std::strlen(payments));
            XDrawString(display, window, gc, 638, 305, reports, std::strlen(reports));

            XSetForeground(display, gc, textColor.pixel);
            const char* memberValue = "124 active";
            const char* trainerValue = "12 available";
            const char* appointmentValue = "18 today";
            const char* subscriptionValue = "109 active";
            const char* paymentValue = "18,430 RON";
            const char* reportValue = "3 alerts";
            XDrawString(display, window, gc, 62, 202, memberValue, std::strlen(memberValue));
            XDrawString(display, window, gc, 350, 202, trainerValue, std::strlen(trainerValue));
            XDrawString(display, window, gc, 638, 202, appointmentValue, std::strlen(appointmentValue));
            XDrawString(display, window, gc, 62, 337, subscriptionValue, std::strlen(subscriptionValue));
            XDrawString(display, window, gc, 350, 337, paymentValue, std::strlen(paymentValue));
            XDrawString(display, window, gc, 638, 337, reportValue, std::strlen(reportValue));

            XSetForeground(display, gc, mutedColor.pixel);
            const char* memberHint = "New this week: 8";
            const char* trainerHint = "On duty now: 7";
            const char* appointmentHint = "Pending: 5";
            const char* subscriptionHint = "Expiring soon: 15";
            const char* paymentHint = "Today: 1,250 RON";
            const char* reportHint = "Summary ready";
            XDrawString(display, window, gc, 62, 224, memberHint, std::strlen(memberHint));
            XDrawString(display, window, gc, 350, 224, trainerHint, std::strlen(trainerHint));
            XDrawString(display, window, gc, 638, 224, appointmentHint, std::strlen(appointmentHint));
            XDrawString(display, window, gc, 62, 359, subscriptionHint, std::strlen(subscriptionHint));
            XDrawString(display, window, gc, 350, 359, paymentHint, std::strlen(paymentHint));
            XDrawString(display, window, gc, 638, 359, reportHint, std::strlen(reportHint));

            XSetForeground(display, gc, cardColor.pixel);
            XFillRectangle(display, window, gc, 36, 420, 826, 135);

            XSetForeground(display, gc, textColor.pixel);
            const char* scheduleTitle = "TODAY'S SCHEDULE";
            const char* scheduleOne = "09:00   Maria Popa        Yoga & Pilates";
            const char* scheduleTwo = "10:30   Alex Ionescu      Strength training";
            const char* scheduleThree = "14:00   Elena Dragomir    Cardio session";
            XDrawString(display, window, gc, 62, 455, scheduleTitle, std::strlen(scheduleTitle));
            XDrawString(display, window, gc, 62, 488, scheduleOne, std::strlen(scheduleOne));
            XDrawString(display, window, gc, 62, 515, scheduleTwo, std::strlen(scheduleTwo));
            XDrawString(display, window, gc, 62, 542, scheduleThree, std::strlen(scheduleThree));

            XSetForeground(display, gc, mutedColor.pixel);
            const char* closeHint = "Press any key, click the window, or use the X button to close.";
            XDrawString(display, window, gc, 36, 592, closeHint, std::strlen(closeHint));
        }
    }

    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}