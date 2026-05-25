#pragma once

#include "../screen.hpp"
#include "../../../engine/mesh_manager.hpp"
#include "../../../include/camera.hpp"

struct GLFWwindow;

class PlaytestScreen : public Screen {
    public:
        PlaytestScreen(GLFWwindow* win, const std::string& projPath = "", MeshManager& meshMgr) 
            : window(win), projectPath(projPath), meshManager(meshMgr) {}
        
        void OnEnter() override;
        void OnExit() override;
        void Update() override;
        void Render() override;

        void SetReturnCallback(std::function<void()> callbackFunc) { onReturn_f = callbackFunc; }
    
    private:
        // Constructor fields
        GLFWwindow* win;
        const std::string& projectPath;
        MeshManager& meshManager;

        // Global playtest fields
        Camera camera;
        float lastDt = 0.0f;
        float voxelSize = 0.1f;

        // Game state
        glm::vec3 playerPosition;

        unsigned int shaderProgram = 0;

        std::function<void()> onReturn_f;
};