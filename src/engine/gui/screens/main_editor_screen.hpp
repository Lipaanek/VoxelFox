#pragma once

#include "../screen.hpp"
#include "../../../include/camera.hpp"
#include "../../../include/mesh.hpp"
#include "../../../engine/mesh_manager.hpp"
#include <string>

struct GLFWwindow;

class MainEditorScreen : public Screen {
    public:
        MainEditorScreen(GLFWwindow* win, const std::string& projPath = "") 
            : window(win), projectPath(projPath) {}

        void OnEnter() override;
        void OnExit() override;
        void Update() override;
        void Render() override;

        // Setter for project path (called after creation)
        void SetProjectPath(const std::string& path) {
            projectPath = path;
        }

    private:
        GLFWwindow* window;
        std::string projectPath;                 // Full path to the project directory
        MeshManager meshManager;                 // Manages all loaded meshes
        unsigned int shaderProgram = 0;
        Camera camera;
        double lastFrameTime = 0.0;
        float voxelSize = 0.1f;                 // Voxel size from project config

        void setUniforms();
};

