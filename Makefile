# Compiler
CXX = g++
CC  = gcc

# Directories
BIN_DIR = bin

# Output
TARGET = $(BIN_DIR)/voxelfox.exe

# Flags
CXXFLAGS = -std=c++20 -Wall -fmodules-ts -Isrc/include -Isrc/thirdparty -Isrc/thirdparty/imgui
CFLAGS   = -Wall -Isrc/include -Isrc/thirdparty -Isrc/thirdparty/imgui

# --- FIX: Added -lole32 and -lcomdlg32 to support tinyfiledialogs ---
LDFLAGS = -Llib -lglfw3dll -lopengl32 -lgdi32 -lole32 -lcomdlg32

# --- Safe Source Gathering ---
ALL_CPP := $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp) $(wildcard src/**/**/**/*.cpp)

# Filter out all ImGui backends, then manually add back only GLFW and OpenGL3
CPP_SRCS := $(filter-out src/thirdparty/imgui/backends/%, $(ALL_CPP)) \
            src/thirdparty/imgui/backends/imgui_impl_glfw.cpp \
            src/thirdparty/imgui/backends/imgui_impl_opengl3.cpp

C_SRCS   := $(wildcard src/*.c)   $(wildcard src/**/*.c)   $(wildcard src/**/**/*.c)   $(wildcard src/**/**/**/*.c)

# Object file generation
OBJS := $(patsubst %.cpp,$(BIN_DIR)/%.o,$(CPP_SRCS)) \
        $(patsubst %.c,$(BIN_DIR)/%.o,$(C_SRCS))

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

# Compile C++ files
$(BIN_DIR)/%.o: %.cpp
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C files
$(BIN_DIR)/%.o: %.c
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	@if exist "$(BIN_DIR)" rmdir /s /q "$(BIN_DIR)"

.PHONY: all clean