#include <memory>
#include <cmath>

#include "core/window/window.hpp"
#include "core/screen/screen_manager.hpp"
#include "scene/basic_screen.hpp"
#include "core/renderer/shader_program.hpp"
#include "core/util/util.hpp"
#include "core/input/input_system.hpp"
#include "core/scripting/lua_engine.hpp"
#include "core/scripting/lua_input_bindings.hpp"

int main() {
    Window window("VoxelFox", 800, 600);

    // Input system and Lua link
    InputSystem input(window.getHandle());
    input.setDefaultBindings();

    LuaEngine lua;
    LuaInputBindings::registerInput(lua.state(), &input);
    lua.loadScript("assets/scripts/test_input.lua");

    // Make screen manager
    ScreenManager screenManager(window);

    // Shader program and shader creation
    ShaderProgram program;
    Shader frag("assets/shaders/3d_scene.frag", ShaderType::Fragment);
    Shader vert("assets/shaders/3d_scene.vert", ShaderType::Vertex);

    // Need to compile the shaders before linking
    frag.compile();
    vert.compile();

    if (frag.getID() == 0 || vert.getID() == 0) {
        Util::Log::error("Failed to compile shaders");
        return 1;
    }
        
    // Attach and link programs
    program.attach(vert);
    program.attach(frag);
    program.link();

    // Compute shader example block
    /* {
        // Load shader
        Shader comp("assets/shaders/square.comp", ShaderType::Compute);
        comp.compile();

        ShaderProgram numSquareProg;
        numSquareProg.attach(comp);
        numSquareProg.link();

        // Fill numbers array
        constexpr int COUNT = 1'000'000;
        std::vector<float> numbers(COUNT);
        for (int i = 0; i < COUNT; i++)
            numbers[i] = static_cast<float>(i + 1);

        // Input buffer, numbers go inside the comp shader
        Buffer inBuf(GL_SHADER_STORAGE_BUFFER);
        inBuf.upload(numbers.data(), COUNT * sizeof(float), GL_DYNAMIC_COPY);

        Buffer outBuf(GL_SHADER_STORAGE_BUFFER);
        outBuf.upload(nullptr, COUNT * sizeof(float), GL_DYNAMIC_READ); // nullptr - alloc, no data

        // Set buffers
        numSquareProg.setUniform("count", COUNT);
        numSquareProg.setStorageBuffer(0, inBuf);  // bind 0 -> input
        numSquareProg.setStorageBuffer(1, outBuf); // bind 1 -> output

        constexpr int LOCAL = 64; // size of threads, matches local_size_x
        numSquareProg.dispatch((COUNT + LOCAL - 1) / LOCAL, 1, 1); // ceil div

        std::vector<float> squaredNums;
        outBuf.read(squaredNums);

        if (squaredNums.size() == COUNT)
            Util::Log::log("All numbers are squared");

        for (float square : squaredNums) {
            if (std::fmod(square, 10000000.0f) == 0.0f) {
                Util::Log::log(std::to_string(square));
            }
        }
    } */

    screenManager.setScreen(std::make_unique<BasicScreen>(program, window));

    // Runs on_ready function
    lua.runOnReady();

    double lastTime = glfwGetTime();
    while (!window.shouldClose()) {
        input.beginInput();
        window.update();

        double now = glfwGetTime();
        float dt = static_cast<float>(now - lastTime);
        lastTime = now;

        lua.runUpdate(dt);
        screenManager.update();

        // Clear screen from previous frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render
        screenManager.render();

        window.present();
    }

    return 0;
}