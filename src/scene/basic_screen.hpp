#pragma once

#include "../core/screen/screen.hpp"
#include "../core/renderer/mesh/mesh.hpp"
#include "../core/camera/camera.hpp"
#include "../core/window/window.hpp"
#include "../core/renderer/light.hpp"
#include "../core/renderer/mesh/scene_mesh_manager.hpp"

class BasicScreen : public Screen {
private:
    ShaderProgram& program;
    SceneLights lights;
    SceneMeshManager meshManager;
    unsigned int voxel2UID = 0;
    size_t sunIndex = 0;
    size_t pointIndex = 0;
    Window& window;
    Camera camera;
    glm::mat4 model { 1.0f };

public:
    BasicScreen(ShaderProgram& program, Window& window) : program(program), window(window) {}

    void render() override;
    void onReady() override;
};
