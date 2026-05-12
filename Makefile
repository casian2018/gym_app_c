CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
IMGUI_DIR := external/imgui
INCLUDES := -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
LIBS := -lX11 -lGL

SOURCES := \
	main.cpp \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_opengl2.cpp

TARGET := gym_app_c

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) $(LIBS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
