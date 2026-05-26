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
    
    private:
        GLFWwindow* window;
        std::string projectPath;
        MeshManager& meshManager;

        // Global playtest fields
        Camera camera;
        float lastDt = 0.0f;
        float voxelSize = 0.1f;

        // Game state
        glm::vec3 playerPosition{0.0f, 0.0f, 0.0f};

        // Rendering
        unsigned int shaderProgram = 0;

        // Input state - mouse look
        double lastMouseX = 0.0;
        double lastMouseY = 0.0;
        bool fpsMouse = true;

        std::function<void()> onReturn_f;
};