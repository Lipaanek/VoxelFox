# Compiler (MinGW cross-compiler for Windows)
CXX = x86_64-w64-mingw32-g++
CC  = x86_64-w64-mingw32-gcc
WINDRES = x86_64-w64-mingw32-windres

# Directories
BIN_DIR = bin

# Output
TARGET = $(BIN_DIR)/voxelfox.exe

# Flags
CXXFLAGS = -std=c++20 -Wall -Isrc/include -Isrc/thirdparty -Isrc/thirdparty/imgui
CFLAGS   = -Wall -Isrc/include -Isrc/thirdparty -Isrc/thirdparty/imgui

LDFLAGS = -static-libstdc++ -static-libgcc -Llib -lglfw3dll -lopengl32 -lgdi32 -lole32 -lcomdlg32

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
	@mkdir -p "$(BIN_DIR)"
	$(CXX) $(OBJS) $(LDFLAGS) -o $@
	cp glfw3.dll "$(BIN_DIR)/glfw3.dll"
	cp -r src/shaders "$(BIN_DIR)/src/shaders"

# Compile C++ files
$(BIN_DIR)/%.o: %.cpp
	@mkdir -p "$(dir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C files
$(BIN_DIR)/%.o: %.c
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	@rm -rf "$(BIN_DIR)"

.PHONY: all clean
