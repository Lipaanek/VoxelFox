#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "main_editor_screen.hpp"
#include "../../../rendering_headers/shader_loader.hpp"
#include "../../../rendering_headers/object_loader.hpp"
#include "../../../rendering_headers/color_loader.hpp"
#include "../../../rendering_headers/voxelizer.hpp"

extern float velocityX;
extern float velocityY;
extern float velocityZ;
extern float yaw;
extern float pitch;

void MainEditorScreen::OnEnter() {
    // Clear any stale OpenGL errors before starting
    while (glGetError() != GL_NO_ERROR) {}

    ObjectLoader loader;
    if (!loader.load("src/meshes/Studanka2.obj")) {
        printf("Failed to load mesh\n");
        return;
    }

    std::vector<Material> materials = loadColorFile("src/color_files/Studanka2.mtl");

    const float voxelSize = 0.1f;
    std::vector<VoxelChunk> voxelChunks = voxelizeGPUCompute(
        loader.getVertices(), loader.getIndices(),
        loader.getTriangleMaterials(), voxelSize);

    VoxelMesh voxelMesh = generateVoxelMesh(voxelChunks, materials);
    voxelRenderMesh = Mesh(voxelMesh.vertices, voxelMesh.indices);

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

    camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
    camera.updateCameraVectors();

    lastFrameTime = glfwGetTime();
}

void MainEditorScreen::OnExit() {
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

void MainEditorScreen::Update() {
    double currentTime = glfwGetTime();
    float dt = static_cast<float>(currentTime - lastFrameTime);
    lastFrameTime = currentTime;

    camera.yaw = yaw;
    camera.pitch = pitch;
    camera.updateCameraVectors();
    camera.updateCameraPosition(velocityX, velocityY, velocityZ, dt);
}

void MainEditorScreen::Render() {
    // Clear stale errors from previous frame (ImGui, etc.)
    while (glGetError() != GL_NO_ERROR) {}

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!shaderProgram) return;

    glUseProgram(shaderProgram);
    setUniforms();
    voxelRenderMesh.draw();

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("OpenGL error in MainEditorScreen::Render: 0x%x\n", err);
    }
}

void MainEditorScreen::setUniforms() {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glm::mat4 projection = camera.getProjectionMatrix(static_cast<float>(fbWidth) / static_cast<float>(fbHeight));

    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");

    glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.position));
    glUniform3f(lightPosLoc, camera.position.x, camera.position.y, camera.position.z);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
}
