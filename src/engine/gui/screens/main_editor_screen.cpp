#include <cmath>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include "main_editor_screen.hpp"
#include "../../../include/shader_loader.hpp"
#include "../../../include/object_loader.hpp"
#include "../../../include/color_loader.hpp"
#include "../../../include/voxelizer.hpp"
#include "../../project_config.hpp"

extern float velocityX;
extern float velocityY;
extern float velocityZ;
extern float yaw;
extern float pitch;

std::vector<Vertex> gridVertices;

void MainEditorScreen::OnEnter() {
    // Clear any stale OpenGL errors before starting
    while (glGetError() != GL_NO_ERROR) {}

    // Load project configuration
    if (!projectPath.empty()) {
        ProjectConfig config = ProjectConfigLoader::Load(projectPath);
        voxelSize = config.voxelSize;
        printf("Loaded project config: name=%s, voxelSize=%.2f\n", config.projectName.c_str(), voxelSize);
    } else {
        printf("Warning: No project path set in MainEditorScreen\n");
        voxelSize = 0.1f; // Use default
    }

    // Load and compile shaders
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

    // Load and compile line grid shaders
    std::string lineVertCode = loadFile("src/shaders/vertex_line.glsl");
    std::string lineFragCode = loadFile("src/shaders/fragment_line.glsl");

    unsigned int lineVertShader = compileShader(GL_VERTEX_SHADER, lineVertCode);
    unsigned int lineFragShader = compileShader(GL_FRAGMENT_SHADER, lineFragCode);

    gridShaderProgram = glCreateProgram();
    glAttachShader(gridShaderProgram, lineVertShader);
    glAttachShader(gridShaderProgram, lineFragShader);
    glLinkProgram(gridShaderProgram);

    glGetProgramiv(gridShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(gridShaderProgram, 512, nullptr, infoLog);
        printf("Grid shader program linking failed: %s\n", infoLog);
        glDeleteProgram(gridShaderProgram);
        gridShaderProgram = 0;
    }

    glDeleteShader(lineVertShader);
    glDeleteShader(lineFragShader);

    camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
    camera.updateCameraVectors();

    lastFrameTime = glfwGetTime();

    const int gridSize = 40;
    const float spacing = 1.0f;

    for (int i = -gridSize; i <= gridSize; i++) {
        glm::vec3 color = (i % 10 == 0)
            ? glm::vec3(0.8f, 0.8f, 0.8f)
            : glm::vec3(0.35f, 0.35f, 0.35f);

        // Vertical lines (parallel to Z)
        gridVertices.emplace_back(
            glm::vec3(i * spacing, 0.0f, -gridSize * spacing),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec2(0.0f),
            color
        );

        gridVertices.emplace_back(
            glm::vec3(i * spacing, 0.0f, gridSize * spacing),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec2(0.0f),
            color
        );

        // Horizontal lines (parallel to X)
        gridVertices.emplace_back(
            glm::vec3(-gridSize * spacing, 0.0f, i * spacing),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec2(0.0f),
            color
        );

        gridVertices.emplace_back(
            glm::vec3(gridSize * spacing, 0.0f, i * spacing),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec2(0.0f),
            color
        );
    }

    setupGridBuffers();
}

void MainEditorScreen::OnExit() {
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
    if (gridShaderProgram) {
        glDeleteProgram(gridShaderProgram);
        gridShaderProgram = 0;
    }
    if (gridVAO) {
        glDeleteVertexArrays(1, &gridVAO);
        gridVAO = 0;
    }
    if (gridVBO) {
        glDeleteBuffers(1, &gridVBO);
        gridVBO = 0;
    }
    gridVertexCount = 0;
}

void drawGui() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Open");
            ImGui::MenuItem("Save");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo");
            ImGui::MenuItem("Redo");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Editor");

    ImGui::BeginChild("LeftPanel", ImVec2(200, 0), true);
    ImGui::Text("Hierarchy");
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightPanel", ImVec2(250, 0), true);
    ImGui::Text("Inspector");
    ImGui::EndChild();

    ImGui::End();
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
    // Clear stale errors from previous frame
    while (glGetError() != GL_NO_ERROR) {}

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!shaderProgram) return;

    glUseProgram(shaderProgram);

    // Set view and projection matrices
    glm::mat4 view = camera.getViewMatrix();
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glm::mat4 projection = camera.getProjectionMatrix(static_cast<float>(fbWidth) / static_cast<float>(fbHeight));

    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.position));
    glUniform3f(lightPosLoc, camera.position.x, camera.position.y, camera.position.z);
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

    int modelLoc = glGetUniformLocation(shaderProgram, "model");

    // Render all loaded meshes
    for (auto& mesh : meshManager.GetLoadedMeshes()) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mesh.modelMatrix));
        mesh.renderMesh.draw();
    }

    drawGrid();
    drawGui();

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

void MainEditorScreen::setupGridBuffers() {
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);

    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    // DYNAMIC_DRAW because the tile content changes every frame
    glBufferData(GL_ARRAY_BUFFER,
                 gridVertices.size() * sizeof(Vertex),
                 gridVertices.data(),
                 GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, uv)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    gridVertexCount = static_cast<GLsizei>(gridVertices.size());
}

// Orientation tile grid
void MainEditorScreen::generateGridTiles(glm::ivec2 tileOrigin) {
    gridVertices.clear();

    const int tileSize = 10;
    const int gridRadius = 4;
    const float spacing = 1.0f;
    const float halfRange = gridRadius * static_cast<float>(tileSize);

    for (int tx = -gridRadius; tx <= gridRadius; tx++) {
        for (int tz = -gridRadius; tz <= gridRadius; tz++) {
            float baseX = tileOrigin.x + tx * tileSize;
            float baseZ = tileOrigin.y + tz * tileSize;

            for (int i = -halfRange; i <= halfRange; i++) {
                // Bold line every 10 units
                bool bold = (i % tileSize == 0);
                glm::vec3 color = bold
                    ? glm::vec3(0.8f, 0.8f, 0.8f)
                    : glm::vec3(0.3f, 0.3f, 0.3f);

                float x  = baseX + i * spacing;
                float z0 = baseZ - halfRange * spacing;
                float z1 = baseZ + halfRange * spacing;

                gridVertices.emplace_back(
                    glm::vec3(x, 0.0f, z0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f), color);
                gridVertices.emplace_back(
                    glm::vec3(x, 0.0f, z1), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f), color);
            }
        }
    }

    for (int tz = -gridRadius; tz <= gridRadius; tz++) {
        for (int tx = -gridRadius; tx <= gridRadius; tx++) {
            float baseX = tileOrigin.x + tx * tileSize;
            float baseZ = tileOrigin.y + tz * tileSize;

            for (int i = -halfRange; i <= halfRange; i++) {
                bool bold = (i % tileSize == 0);
                glm::vec3 color = bold
                    ? glm::vec3(0.8f, 0.8f, 0.8f)
                    : glm::vec3(0.3f, 0.3f, 0.3f);

                float x0 = baseX - halfRange * spacing;
                float x1 = baseX + halfRange * spacing;
                float z  = baseZ + i * spacing;

                gridVertices.emplace_back(
                    glm::vec3(x0, 0.0f, z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f), color);
                gridVertices.emplace_back(
                    glm::vec3(x1, 0.0f, z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f), color);
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 gridVertices.size() * sizeof(Vertex),
                 gridVertices.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    gridVertexCount = static_cast<GLsizei>(gridVertices.size());
}

void MainEditorScreen::drawGrid() {
    if (!gridShaderProgram || gridVAO == 0) return;

    // Rebuild tile grid whenever the camera crosses into a new 10 unit cell
    glm::ivec2 tileOrigin(
        static_cast<int>(std::floor(camera.position.x / 10.0f)) * 10,
        static_cast<int>(std::floor(camera.position.z / 10.0f)) * 10
    );
    if (tileOrigin != prevTileOrigin) {
        prevTileOrigin = tileOrigin;
        generateGridTiles(tileOrigin);
    }

    glm::mat4 view = camera.getViewMatrix();
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glm::mat4 projection = camera.getProjectionMatrix(static_cast<float>(fbWidth) / static_cast<float>(fbHeight));

    glUseProgram(gridShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(gridShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(gridShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(gridShaderProgram, "viewPos"), 1, glm::value_ptr(camera.position));
    glUniform1f(glGetUniformLocation(gridShaderProgram, "fadeNear"), 0.0f);
    glUniform1f(glGetUniformLocation(gridShaderProgram, "fadeFar"), 65.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, gridVertexCount);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
