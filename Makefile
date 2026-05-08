# variables
CXX = g++
CC = gcc
CXXFLAGS = -std=c++20 -Wall -Iinclude -Isrc/include -fmodules-ts
CFLAGS = -Wall -Iinclude -Isrc/include
SRC_DIR  = src
BIN_DIR  = bin
TARGET   = $(BIN_DIR)/voxelfox.exe
LDFLAGS = -Llib -lglfw3dll -lopengl32 -lgdi32

# Find all source files
CPP_SRCS = $(shell dir /s /b $(SRC_DIR)\*.cpp)
C_SRCS   = $(shell dir /s /b $(SRC_DIR)\*.c)

# Object files from sources
OBJS = $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)
C_OBJS = $(C_SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)

# All objects to link (C++, then C)
ALL_OBJS = $(OBJS) $(C_OBJS)

# Default target
all: $(TARGET)

# Create bin directory and link
$(TARGET): $(ALL_OBJS)
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	$(CXX) $(CXXFLAGS) $(ALL_OBJS) $(LDFLAGS) -o $(TARGET)

# Compile .cppm module interface files first (generates BMI in bin/gcm.cache)
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cppm
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile .cpp files into .o files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile .c files into .o files
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	@if exist "$(BIN_DIR)" rmdir /s /q "$(BIN_DIR)"
	@if exist "gcm.cache" rmdir /s /q "gcm.cache"

.PHONY: all clean