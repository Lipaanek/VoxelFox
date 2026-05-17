#pragma once

#include "../screen.hpp"
#include "../../../rendering_headers/camera.hpp"
#include "../../../rendering_headers/mesh.hpp"

struct GLFWwindow;

class MainEditorScreen : public Screen {
    public:
        MainEditorScreen(GLFWwindow* win) : window(win) {}

        void OnEnter() override;
        void OnExit() override;
        void Update() override;
        void Render() override;

    private:
        GLFWwindow* window;
        Mesh voxelRenderMesh;
        unsigned int shaderProgram = 0;
        Camera camera;
        double lastFrameTime = 0.0;

        void setUniforms();
};
