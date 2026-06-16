#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <string>

#include "playtest_screen.hpp"
#include "../../../include/shader_loader.hpp"
#include "../../../include/loaded_mesh.hpp"
#include "../../../engine/scene_hierarchy/nodes/mesh_instance_3d.hpp"
#include "../../project_config.hpp"

extern float velocityX;
extern float velocityY;
extern float velocityZ;

void PlaytestScreen::OnEnter() {
    while (glGetError() != GL_NO_ERROR) {}

    if (!projectPath.empty()) {
        ProjectConfig config = ProjectConfigLoader::Load(projectPath);
        voxelSize = config.voxelSize;
    }

    std::string vertexCode = loadFile("src/shaders/vertex.glsl");
    std::string fragmentCode = loadFile("src/shaders/fragment.glsl");

    unsigned int vertShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        printf("Shader program linking failed: %s\n", infoLog);
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    camera.position = glm::vec3(0.0f, 1.0f, 3.0f);
    camera.updateCameraVectors();

    mouseCaptured = true;
    firstMouse = true;
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void PlaytestScreen::OnExit() {
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

void PlaytestScreen::Update() {
    double currentTime = glfwGetTime();
    float dt = static_cast<float>(currentTime - lastDt);
    if (lastDt == 0.0f) dt = 0.0f;
    lastDt = currentTime;

    scene.Update(dt);

    if (mouseCaptured) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        static float playYaw = 0.0f;
        static float playPitch = 0.0f;
        const float sens = 0.1f;

        if (firstMouse) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
        }

        float dx = static_cast<float>(lastMouseX - mouseX);
        float dy = static_cast<float>(lastMouseY - mouseY);

        playYaw += dx * sens;
        playPitch += dy * sens;
        playPitch = glm::clamp(playPitch, -89.0f, 89.0f);

        lastMouseX = mouseX;
        lastMouseY = mouseY;

        camera.yaw = playYaw;
        camera.pitch = playPitch;
        camera.updateCameraVectors();
    }

    camera.updateCameraPosition(velocityX, velocityY, velocityZ, dt);

    if (camera.position.y < 0.5f) {
        camera.position.y = 0.5f;
    }

    static bool escapeWasPressed = false;
    bool escapePressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (escapePressed && !escapeWasPressed) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (onReturn_f) {
            onReturn_f();
        }
    }
    escapeWasPressed = escapePressed;
}

void PlaytestScreen::Render() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!shaderProgram) return;

    glUseProgram(shaderProgram);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(
        fbWidth > 0 && fbHeight > 0 ? 
        static_cast<float>(fbWidth) / static_cast<float>(fbHeight) : 1.0f
    );

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(camera.position));
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 0.0f, 10.0f, 0.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");

    for (auto* meshNode : scene.GetAllMeshInstances()) {
        LoadedMesh* meshData = meshManager.GetMeshByPath(meshNode->meshAssetPath);
        if (meshData) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(meshNode->GetGlobalTransform()));
            meshData->renderMesh.draw();
        }
    }
}
