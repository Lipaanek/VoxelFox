CXX        = x86_64-w64-mingw32-g++
CC         = x86_64-w64-mingw32-gcc
CXXFLAGS   = -std=c++20 -Wall -Wextra -Wpedantic
CFLAGS     = -std=c11 -Wall -Wextra -Wpedantic
INCLUDES   = -Ithirdparty -Ithirdparty/glad -Ithirdparty/GLFW -Ithirdparty/glm -Ithirdparty/tinyfiledialogs
LDFLAGS    = -Lthirdparty
LDLIBS     = -lglfw3dll -lopengl32 -lgdi32 -lole32 -lcomdlg32

BUILDDIR   = build
OBJDIR     = $(BUILDDIR)/obj
TARGET     = $(BUILDDIR)/voxelfox.exe
TESTTARGET = $(BUILDDIR)/voxelfox_tests.exe
TINYFD_SRC = thirdparty/tinyfiledialogs/tinyfiledialogs.c
TINYFD_OBJ = $(OBJDIR)/tinyfiledialogs.o

RUNNER     ?=

# --- Add new source files to SRCS below ---
SRCS = \
	src/main.cpp \
	src/scene/basic_screen.cpp \
	src/scene/screen_manager.cpp \
	src/core/window.cpp \
	src/renderer/shader.cpp \
	src/util/util.cpp \

OBJS = $(SRCS:src/%.cpp=$(OBJDIR)/%.o)

TEST_SRCS =

TEST_OBJS = $(TEST_SRCS:tests/%.cpp=$(OBJDIR)/%.o)

ifeq ($(OS),Windows_NT)
  MKDIR = mkdir $(subst /,\,$(1)) 2>nul
  RMDIR = rmdir /s /q $(subst /,\,$(1)) 2>nul
else
  MKDIR = mkdir -p $(1)
  RMDIR = rm -rf $(1)
endif

DIRS = $(sort $(dir $(OBJS) $(TINYFD_OBJ)))
$(shell $(call MKDIR,$(DIRS)))

.PHONY: all clean help run test

all: $(TARGET)

$(TARGET): $(OBJS) $(TINYFD_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TINYFD_OBJ): $(TINYFD_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(TESTTARGET)

$(TESTTARGET): $(TEST_OBJS) $(filter-out %/main.o,$(OBJS)) $(TINYFD_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJDIR)/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	$(RUNNER) $(TARGET)

clean:
	$(call RMDIR,$(BUILDDIR))

help:
	@echo "Targets: all test run clean help"
	@echo ""
	@echo "Adding a new source file:"
	@echo "  1. Add 'src/subdir/file.cpp \' to SRCS (backslash to continue)"
	@echo "  2. Subdirectories under $(OBJDIR) are created automatically"
	@echo ""
	@echo "For tests, add 'tests/test_foo.cpp' to TEST_SRCS."
