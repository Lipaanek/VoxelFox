# Compiler
CXX = g++
CC  = gcc

# Directories
SRC_DIR = src
BIN_DIR = bin
INC_DIR = include

# Output
TARGET = $(BIN_DIR)/voxelfox.exe

# Flags
CXXFLAGS = -std=c++20 -Wall -fmodules-ts -Iinclude -Iinclude/imgui
CFLAGS   = -Wall -Iinclude -Iinclude/imgui

LDFLAGS = -Llib -lglfw3dll -lopengl32 -lgdi32

# ImGui sources
IMGUI_SRCS = \
	include/imgui/imgui.cpp \
	include/imgui/imgui_draw.cpp \
	include/imgui/imgui_widgets.cpp \
	include/imgui/imgui_tables.cpp \
	include/imgui/backends/imgui_impl_glfw.cpp \
	include/imgui/backends/imgui_impl_opengl3.cpp

# Source files (from src/ directory only)
CPP_SRCS := $(shell powershell -Command "Get-ChildItem -Path src -Recurse -Filter *.cpp | Resolve-Path -Relative")
C_SRCS   := $(shell powershell -Command "Get-ChildItem -Path src -Recurse -Filter *.c | Resolve-Path -Relative")

CPP_SRCS := $(subst \,/,$(CPP_SRCS))
C_SRCS   := $(subst \,/,$(C_SRCS))

CPP_SRCS += $(IMGUI_SRCS)

# Object files
OBJS := $(patsubst %.cpp,$(BIN_DIR)/%.o,$(CPP_SRCS))
OBJS += $(patsubst %.c,$(BIN_DIR)/%.o,$(C_SRCS))

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

# Compile C++
$(BIN_DIR)/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C
$(BIN_DIR)/%.o: %.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile modules
$(BIN_DIR)/%.o: %.cppm
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	@if exist "$(BIN_DIR)" rmdir /s /q "$(BIN_DIR)"
	@if exist "gcm.cache" rmdir /s /q "gcm.cache"

.PHONY: all clean