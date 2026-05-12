// main.cpp
// Purpose: Dear ImGui pop-up dashboard for a Gym Management System.
// Author: Your Name

#include "imgui.h"
#include "backends/imgui_impl_opengl2.h"

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <chrono>
#include <iostream>

int main()
{
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        std::cerr << "Could not open a graphical display.\n";
        return 1;
    }

    int screen = DefaultScreen(display);
    int visualAttributes[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 24, None};
    XVisualInfo* visualInfo = glXChooseVisual(display, screen, visualAttributes);
    if (visualInfo == nullptr) {
        std::cerr << "Could not create an OpenGL visual.\n";
        XCloseDisplay(display);
        return 1;
    }

    Colormap colorMap = XCreateColormap(display, RootWindow(display, screen), visualInfo->visual, AllocNone);
    XSetWindowAttributes windowAttributes{};
    windowAttributes.colormap = colorMap;
    windowAttributes.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask | ButtonPressMask;

    int windowWidth = 1000;
    int windowHeight = 680;

    Window window = XCreateWindow(
        display,
        RootWindow(display, screen),
        120,
        80,
        windowWidth,
        windowHeight,
        0,
        visualInfo->depth,
        InputOutput,
        visualInfo->visual,
        CWColormap | CWEventMask,
        &windowAttributes
    );

    XStoreName(display, window, "Gym Management System - ImGui");
    Atom deleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteMessage, 1);

    GLXContext glContext = glXCreateContext(display, visualInfo, nullptr, GL_TRUE);
    if (glContext == nullptr) {
        std::cerr << "Could not create an OpenGL context.\n";
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        return 1;
    }

    glXMakeCurrent(display, window, glContext);
    XMapWindow(display, window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.DisplaySize = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.ChildRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 6.0f;

    ImGui_ImplOpenGL2_Init();

    bool running = true;
    auto previousTime = std::chrono::steady_clock::now();

    while (running) {
        while (XPending(display) > 0) {
            XEvent event;
            XNextEvent(display, &event);

            if (event.type == ClientMessage && static_cast<Atom>(event.xclient.data.l[0]) == deleteMessage) {
                running = false;
            }

            if (event.type == ConfigureNotify) {
                windowWidth = event.xconfigure.width;
                windowHeight = event.xconfigure.height;
                io.DisplaySize = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHeight));
            }

            if (event.type == KeyPress && event.xkey.keycode == 9) {
                running = false;
            }
        }

        auto currentTime = std::chrono::steady_clock::now();
        io.DeltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;

        ImGui_ImplOpenGL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::Begin(
            "Gym Dashboard",
            nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );

        ImGui::Text("GYM MANAGEMENT SYSTEM");
        ImGui::TextDisabled("Dear ImGui visual dashboard");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Columns(3, nullptr, false);

        ImGui::BeginChild("membersCard", ImVec2(0, 118), true);
        ImGui::TextDisabled("MEMBERS");
        ImGui::Text("124 active");
        ImGui::ProgressBar(0.82f, ImVec2(-1, 16), "82%");
        ImGui::TextDisabled("8 new this week");
        ImGui::EndChild();
        ImGui::NextColumn();

        ImGui::BeginChild("trainersCard", ImVec2(0, 118), true);
        ImGui::TextDisabled("TRAINERS");
        ImGui::Text("12 available");
        ImGui::ProgressBar(0.58f, ImVec2(-1, 16), "7 on duty");
        ImGui::TextDisabled("Strength, yoga, cardio");
        ImGui::EndChild();
        ImGui::NextColumn();

        ImGui::BeginChild("appointmentsCard", ImVec2(0, 118), true);
        ImGui::TextDisabled("APPOINTMENTS");
        ImGui::Text("18 today");
        ImGui::ProgressBar(0.72f, ImVec2(-1, 16), "5 pending");
        ImGui::TextDisabled("Next: 09:00 Maria Popa");
        ImGui::EndChild();
        ImGui::NextColumn();

        ImGui::BeginChild("subscriptionsCard", ImVec2(0, 118), true);
        ImGui::TextDisabled("SUBSCRIPTIONS");
        ImGui::Text("109 active");
        ImGui::ProgressBar(0.88f, ImVec2(-1, 16), "15 expiring");
        ImGui::TextDisabled("Monthly, quarterly, annual");
        ImGui::EndChild();
        ImGui::NextColumn();

        ImGui::BeginChild("paymentsCard", ImVec2(0, 118), true);
        ImGui::TextDisabled("PAYMENTS");
        ImGui::Text("18,430 RON");
        ImGui::ProgressBar(0.64f, ImVec2(-1, 16), "1,250 today");
        ImGui::TextDisabled("Cash, card, transfer");
        ImGui::EndChild();
        ImGui::NextColumn();

        ImGui::BeginChild("reportsCard", ImVec2(0, 118), true);
        ImGui::TextDisabled("REPORTS");
        ImGui::Text("Summary ready");
        ImGui::ProgressBar(0.35f, ImVec2(-1, 16), "3 alerts");
        ImGui::TextDisabled("Subscriptions need attention");
        ImGui::EndChild();

        ImGui::Columns(1);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Today's Schedule");
        if (ImGui::BeginTable("scheduleTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Time");
            ImGui::TableSetupColumn("Member");
            ImGui::TableSetupColumn("Trainer");
            ImGui::TableSetupColumn("Session");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("09:00");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Maria Popa");
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("Ioana Yoga");
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("Yoga & Pilates");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("10:30");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Alex Ionescu");
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("Bogdan Fitness");
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("Strength training");

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("14:00");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("Elena Dragomir");
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("Radu Cardio");
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("Cardio session");

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::TextDisabled("Close the window or press Esc to exit.");
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0.94f, 0.96f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        glXSwapBuffers(display, window);
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui::DestroyContext();
    glXMakeCurrent(display, None, nullptr);
    glXDestroyContext(display, glContext);
    XDestroyWindow(display, window);
    XFreeColormap(display, colorMap);
    XFree(visualInfo);
    XCloseDisplay(display);

    return 0;
}
