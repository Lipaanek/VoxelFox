CXX        = x86_64-w64-mingw32-g++
CC         = x86_64-w64-mingw32-gcc
CXXFLAGS   = -std=c++20 -Wall -Wextra -Wpedantic -MMD -MP
CFLAGS     = -std=c11 -Wall -Wextra -Wpedantic -MMD -MP
THIRDPARTY_CFLAGS = -std=c11 -MMD -MP
INCLUDES   = -Isrc -Ithirdparty -Ithirdparty/glad -Ithirdparty/GLFW -Ithirdparty/glm -Ithirdparty/tinyfiledialogs -Ithirdparty/lua
LDFLAGS    = -Lthirdparty
LDLIBS     = -lglfw3dll -lopengl32 -lgdi32 -lole32 -lcomdlg32

BUILDDIR   = build
OBJDIR     = $(BUILDDIR)/obj
TARGET     = $(BUILDDIR)/voxelfox.exe
TESTTARGET = $(BUILDDIR)/voxelfox_tests.exe
TINYFD_SRC = thirdparty/tinyfiledialogs/tinyfiledialogs.c
TINYFD_OBJ = $(OBJDIR)/tinyfiledialogs.o
GLAD_SRC  = thirdparty/glad/glad.c
GLAD_OBJ  = $(OBJDIR)/glad.o
LUA_SRCS = $(wildcard thirdparty/lua/*.c)
LUA_OBJS = $(LUA_SRCS:thirdparty/lua/%.c=$(OBJDIR)/lua/%.o)
CATCH2_INCLUDES = -Ithirdparty/catch2
CATCH2_OBJ = $(OBJDIR)/catch_amalgamated.o

RUNNER     ?=

# Default goal must be set before the first rule; -include of .d files
# above would otherwise make an up-to-date .o file the default target.
.DEFAULT_GOAL := all

# --- Add new source files to SRCS below ---
SRCS = \
	src/main.cpp \
	src/scene/basic_scene.cpp \
	src/core/scene/scene_manager.cpp \
	src/core/scene/editor.cpp \
	src/core/scene/playtest.cpp \
	src/core/window/window.cpp \
	src/core/renderer/shader.cpp \
	src/core/renderer/shader_program.cpp \
	src/core/renderer/buffer.cpp \
	src/core/renderer/voxelizer.cpp \
	src/core/renderer/vertex_array.cpp \
	src/core/renderer/mesh/mesh.cpp \
	src/core/renderer/mesh/mesh_manager.cpp \
	src/core/renderer/mesh/mesh_renderer.cpp \
	src/core/camera/camera.cpp \
	src/core/input/action_map.cpp \
	src/core/input/input_system.cpp \
	src/core/scripting/lua_engine.cpp \
	src/core/scripting/lua_flags.cpp \
	src/core/scripting/lua_script.cpp \
	src/core/scripting/inputs/lua_input_bindings.cpp \
	src/core/scripting/camera/lua_camera.cpp \
	src/core/scripting/vector/lua_vector3.cpp \
	src/core/util/util.cpp \
	src/core/lighting/light.cpp \
	src/core/lighting/lighting.cpp \
	src/core/model_loading/obj_loader.cpp \
	src/nodes/voxel.cpp \
	src/nodes/mesh_instance_3d.cpp \
	src/nodes/node3d.cpp \
	src/nodes/node.cpp \

OBJS = $(SRCS:src/%.cpp=$(OBJDIR)/%.o)

TEST_SRCS = \
	tests/test_util.cpp \
	tests/test_camera.cpp \
	tests/test_obj_loader.cpp \
	tests/test_action_map.cpp \


TEST_OBJS = $(TEST_SRCS:tests/%.cpp=$(OBJDIR)/%.o)

ifeq ($(OS),Windows_NT)
  MKDIR = mkdir $(subst /,\,$(1)) 2>nul
  RMDIR = rmdir /s /q $(subst /,\,$(1)) 2>nul
  COPY = copy /Y
  WINPATH = $(subst /,\,$(1))
else
  MKDIR = mkdir -p $(1)
  RMDIR = rm -rf $(1)
  COPY = cp
  WINPATH = $(1)
endif

DIRS = $(sort $(dir $(OBJS) $(TINYFD_OBJ) $(LUA_OBJS)))
$(shell $(call MKDIR,$(DIRS)))

-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)
-include $(TINYFD_OBJ:.o=.d)
-include $(GLAD_OBJ:.o=.d)
-include $(LUA_OBJS:.o=.d)
-include $(CATCH2_OBJ:.o=.d)

GLFW_COPIES = $(BUILDDIR)/glfw3.dll $(BUILDDIR)/libglfw3dll.a

$(BUILDDIR)/glfw3.dll: thirdparty/glfw3.dll
	-$(call MKDIR,$(BUILDDIR))
	$(COPY) $(call WINPATH,$<) $(call WINPATH,$@)

$(BUILDDIR)/libglfw3dll.a: thirdparty/libglfw3dll.a
	-$(call MKDIR,$(BUILDDIR))
	$(COPY) $(call WINPATH,$<) $(call WINPATH,$@)

.PHONY: all clean help run test

all: $(TARGET) $(GLFW_COPIES)

$(TARGET): $(OBJS) $(TINYFD_OBJ) $(GLAD_OBJ) $(LUA_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TINYFD_OBJ): $(TINYFD_SRC)
	$(CC) $(THIRDPARTY_CFLAGS) $(INCLUDES) -c $< -o $@

$(GLAD_OBJ): $(GLAD_SRC)
	$(CC) $(THIRDPARTY_CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/lua/%.o: thirdparty/lua/%.c
	$(CC) $(THIRDPARTY_CFLAGS) $(INCLUDES) -c $< -o $@

$(CATCH2_OBJ): thirdparty/catch2/catch_amalgamated.cpp
	$(CXX) -std=c++20 $(CATCH2_INCLUDES) -c $< -o $@

test: $(TESTTARGET) $(GLFW_COPIES)
	@echo "=== Running tests ==="
	$(RUNNER) $(TESTTARGET)

$(TESTTARGET): $(TEST_OBJS) $(CATCH2_OBJ) $(filter-out %/main.o,$(OBJS)) $(TINYFD_OBJ) $(GLAD_OBJ) $(LUA_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(OBJDIR)/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(CATCH2_INCLUDES) -Isrc -c $< -o $@

run: $(TARGET) $(GLFW_COPIES)
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
