#pragma once

#include "../screen.hpp"
#include "../../../include/camera.hpp"
#include "../../../include/mesh.hpp"
#include "../../../engine/mesh_manager.hpp"
#include <string>
#include <vector>

struct GLFWwindow;

class MainEditorScreen : public Screen {
    public:
        MainEditorScreen(GLFWwindow* win, MeshManager& meshMgr, const std::string& projPath = "") 
            : window(win), meshManager(meshMgr), projectPath(projPath) {}
        
        void OnEnter() override;
        void OnExit() override;
        void Update() override;
        void Render() override;

        void SetProjectPath(const std::string& path) {
            projectPath = path;
        }

        std::string GetProjectPath() const {
            return projectPath;
        }

        std::function<void()> onEnterPlaytest_f;

     private:
        GLFWwindow* window;

        std::string projectPath;

        MeshManager& meshManager;

        unsigned int shaderProgram = 0;
        unsigned int gridVAO = 0;
        unsigned int gridVBO = 0;
        unsigned int gridShaderProgram = 0;
        GLsizei gridVertexCount = 0;

        Camera camera;

        double lastFrameTime = 0.0;
        float voxelSize = 0.1f;
        glm::ivec2 prevTileOrigin = glm::ivec2(0x7fffffff, 0x7fffffff);

        // Voxelization state
        bool voxelizing = false;
        bool startVoxelization = false;
        std::string g_selectedFileInfo;
        std::string voxelizeError;

        void setUniforms();
        void setupGridBuffers();
        void generateGridTiles(glm::ivec2 tileOrigin);
        void drawGrid();
        void voxelizeSelectedMesh();
};

