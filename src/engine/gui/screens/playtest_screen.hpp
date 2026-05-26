#pragma once

#include "../screen.hpp"
#include "../../../engine/mesh_manager.hpp"
#include "../../../include/camera.hpp"

#include <functional>
#include <string>
#include <vector>

struct GLFWwindow;

class PlaytestScreen : public Screen {
    public:
        PlaytestScreen(GLFWwindow* win, const std::string& projPath, MeshManager& meshMgr) 
            : window(win), projectPath(projPath), meshManager(meshMgr) {}
        
        void OnEnter() override;
        void OnExit() override;
        void Update() override;
        void Render() override;

        void SetReturnCallback(std::function<void()> callbackFunc) { onReturn_f = callbackFunc; }
        void SetProjectPath(const std::string& path) { projectPath = path; }

        bool IsMouseCaptured() const { return mouseCaptured; }

        void HandleRMB(bool pressed) {
            if (pressed && !rmbWasPressedLastFrame) {
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                glfwSetCursorPos(window, width / 2, height / 2);

                mouseCaptured = !mouseCaptured;
                if (mouseCaptured) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    firstMouse = true;
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
            rmbWasPressedLastFrame = pressed;
        }

     private:
        GLFWwindow* window;
        std::string projectPath;
        MeshManager& meshManager;

        // Game state
        Camera camera;
        float lastDt = 0.0f;
        float voxelSize = 0.1f;

        // Rendering
        unsigned int shaderProgram = 0;

        // Input state - mouse look with toggle
        double lastMouseX = 0.0;
        double lastMouseY = 0.0;
        bool mouseCaptured = true;
        bool firstMouse = true;
        bool rmbWasPressedLastFrame = false;

        std::function<void()> onReturn_f;
};