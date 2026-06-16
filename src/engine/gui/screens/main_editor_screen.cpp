#include <cmath>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <filesystem>
#include <string>
#include <tinyfiledialogs/tinyfiledialogs.h>

namespace fs = std::filesystem;

#include "main_editor_screen.hpp"
#include "../../../include/shader_loader.hpp"
#include "../../../include/object_loader.hpp"
#include "../../../include/color_loader.hpp"
#include "../../../include/voxelizer.hpp"
#include "../../../include/loaded_mesh.hpp"
#include "../../../engine/scene_hierarchy/nodes/mesh_instance_3d.hpp"
#include "../../project_config.hpp"

extern float velocityX;
extern float velocityY;
extern float velocityZ;
extern float yaw;
extern float pitch;

std::vector<Vertex> gridVertices;

std::string objFile;
std::string matFile;

static fs::path g_rootPath;
static fs::path g_selectedPath;

void MainEditorScreen::OnEnter() {
    while (glGetError() != GL_NO_ERROR) {}

    if (!projectPath.empty()) {
        ProjectConfig config = ProjectConfigLoader::Load(projectPath);
        g_rootPath = projectPath;
        voxelSize = config.voxelSize;
        printf("Loaded project config: name=%s, voxelSize=%.2f\n", config.projectName.c_str(), voxelSize);
    } else {
        printf("Warning: No project path set in MainEditorScreen\n");
        voxelSize = 0.1f;
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
    setupSelectionBox();
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
    if (selectionVAO) {
        glDeleteVertexArrays(1, &selectionVAO);
        selectionVAO = 0;
    }
    if (selectionVBO) {
        glDeleteBuffers(1, &selectionVBO);
        selectionVBO = 0;
    }
    gridVertexCount = 0;
    meshManager.Clear();
}

void DrawFileNode(const fs::path& path) {
    std::string name = path.filename().string();

    bool isDir = fs::is_directory(path);

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    if (!isDir)
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    if (path == g_selectedPath)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool open = false;

    if (isDir)
        open = ImGui::TreeNodeEx(name.c_str(), flags);
    else
        ImGui::TreeNodeEx(name.c_str(), flags, "%s", name.c_str());

    if (ImGui::IsItemClicked())
        g_selectedPath = path;

    if (!isDir && path.extension() == ".voxf") {
        if (ImGui::BeginDragDropSource()) {
            std::string absPath = path.string();
            ImGui::SetDragDropPayload("VOXEL_MODEL", absPath.c_str(), absPath.size() + 1);
            ImGui::Text("Add %s", path.filename().string().c_str());
            ImGui::EndDragDropSource();
        }
    }

    if (isDir && open) {
        for (auto& entry : fs::directory_iterator(path)) {
            DrawFileNode(entry.path());
        }
        ImGui::TreePop();
    }
}

void ImportModel(const std::string& objFile, const std::string& matFile, 
                 const std::string& projectPath, MeshManager& meshManager, 
                 float voxelSize, Scene& scene) {
    fs::path destDir = fs::path(projectPath) / "assets" / "objects";
    fs::create_directories(destDir);

    fs::path objSrc(objFile);
    fs::path objDest = destDir / objSrc.filename();
    fs::copy(objSrc, objDest, fs::copy_options::overwrite_existing);

    fs::path mtlSrc(matFile);
    if (fs::exists(mtlSrc)) {
        fs::path mtlDest = destDir / mtlSrc.filename();
        fs::copy(mtlSrc, mtlDest, fs::copy_options::overwrite_existing);
    }

    std::string relativePath = objSrc.filename().string();
    meshManager.LoadMesh(projectPath, relativePath, voxelSize);

    auto meshNode = std::make_unique<MeshInstance3D>();
    meshNode->name = relativePath;
    meshNode->SetMesh(relativePath, meshManager);
    meshNode->selected = true;
    scene.GetRoot()->AddChild(std::move(meshNode));
}

void drawGui(const std::string& projectPath, MeshManager& meshManager, 
             float voxelSize, Scene& scene) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Open");
            if (ImGui::MenuItem("Save")) {
                scene.Save((fs::path(projectPath) / "scene.vsf").string());
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo");
            ImGui::MenuItem("Redo");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::SetNextWindowSizeConstraints(
        ImVec2(200, 400),
        ImVec2(FLT_MAX, FLT_MAX)
    );
    ImGui::Begin("File Explorer");

    if (fs::exists(g_rootPath)) {
        DrawFileNode(g_rootPath);
    } else {
        ImGui::Text("Invalid root path");
    }

    ImGui::Separator();
    ImGui::Text("Selected: %s", g_selectedPath.string().c_str());

    ImGui::End();

    ImGui::Begin("Model Loader");
    if (ImGui::Button("Select Object File")) {
        const char* filterPatterns[] = {"*.obj"};
        const char* path = tinyfd_openFileDialog(
            "Select .obj File",
            "",
            1,
            filterPatterns,
            "Object Files",
            0
        );

        if (path) {
            objFile = path;
        }
    }
    if (ImGui::Button("Select Material File")) {
        const char* filterPatterns[] = {"*.mtl"};
        const char* path = tinyfd_openFileDialog(
            "Select .mtl File",
            "",
            1,
            filterPatterns,
            "Material Files",
            0
        );

        if (path) {
            matFile = path;
        }
    }
    if (ImGui::Button("Import")) {
        if (!objFile.empty() && !matFile.empty()) {
            ImportModel(objFile, matFile, projectPath, meshManager, voxelSize, scene);
        }
    }
    ImGui::End();

    ImGui::Begin("Model Properties");
    MeshInstance3D* selected = scene.GetSelectedMeshInstance();
    if (selected) {
        selected->BuildUI();
    } else {
        ImGui::Text("No model selected");
    }
    ImGui::End();

    ImGui::Begin("Fast Import");
    ImGui::Text("Drag and Drop");
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("VOXEL_MODEL")) {
            std::string voxfPath((const char*)payload->Data);
            fs::path root = fs::path(projectPath) / "assets" / "objects";
            std::string relPath = fs::relative(voxfPath, root).string();

            meshManager.LoadVoxf(projectPath, relPath);

            auto meshNode = std::make_unique<MeshInstance3D>();
            meshNode->name = relPath;
            meshNode->SetMesh(relPath, meshManager);
            meshNode->selected = true;
            scene.GetRoot()->AddChild(std::move(meshNode));
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::End();
}

void MainEditorScreen::Update() {
    double currentTime = glfwGetTime();
    float dt = static_cast<float>(currentTime - lastFrameTime);
    lastFrameTime = currentTime;

    scene.Update(dt);

    static bool f5PressedLastFrame = false;
    bool f5Pressed = glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS;

    if (f5Pressed && !f5PressedLastFrame) {
        if (onEnterPlaytest_f)
            onEnterPlaytest_f();
    }
    
    f5PressedLastFrame = f5Pressed;

    bool leftDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (leftDown && !leftButtonHeld && !ImGui::GetIO().WantCaptureMouse) {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        handleModelClick(mx, my);
    }
    leftButtonHeld = leftDown;

    camera.yaw = yaw;
    camera.pitch = pitch;
    camera.updateCameraVectors();
    camera.updateCameraPosition(velocityX, velocityY, velocityZ, dt);
}

void MainEditorScreen::Render() {
    while (glGetError() != GL_NO_ERROR) {}

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!shaderProgram) return;

    glUseProgram(shaderProgram);

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

    for (auto* meshNode : scene.GetAllMeshInstances()) {
        LoadedMesh* meshData = meshManager.GetMeshByPath(meshNode->meshAssetPath);
        if (meshData) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(meshNode->GetGlobalTransform()));
            meshData->renderMesh.draw();
        }
    }

    for (auto* meshNode : scene.GetAllMeshInstances()) {
        if (meshNode->selected) {
            drawSelectionBox(meshNode);
        }
    }

    drawGrid();
    drawGui(projectPath, meshManager, voxelSize, scene);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("OpenGL error in MainEditorScreen::Render: 0x%x\n", err);
    }
}

void MainEditorScreen::setupGridBuffers() {
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);

    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
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
    glm::mat4 identity(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(gridShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(identity));
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

void MainEditorScreen::setupSelectionBox() {
    const float h = 0.5f;
    glm::vec3 corners[8] = {
        {-h, -h, -h}, { h, -h, -h}, { h, -h,  h}, {-h, -h,  h},
        {-h,  h, -h}, { h,  h, -h}, { h,  h,  h}, {-h,  h,  h}
    };
    int edges[24] = {
        0,1, 1,2, 2,3, 3,0,
        4,5, 5,6, 6,7, 7,4,
        0,4, 1,5, 2,6, 3,7
    };

    glm::vec3 color(1.0f, 1.0f, 0.0f);
    std::vector<Vertex> verts;
    verts.reserve(24);
    for (int i = 0; i < 24; i++) {
        verts.emplace_back(corners[edges[i]], glm::vec3(0.0f), glm::vec2(0.0f), color);
    }

    glGenVertexArrays(1, &selectionVAO);
    glGenBuffers(1, &selectionVBO);

    glBindVertexArray(selectionVAO);
    glBindBuffer(GL_ARRAY_BUFFER, selectionVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

bool MainEditorScreen::rayAABBIntersect(const glm::vec3& origin, const glm::vec3& dir,
                                         const glm::vec3& bmin, const glm::vec3& bmax, float& t) const {
    float tMin = -INFINITY, tMax = INFINITY;
    for (int i = 0; i < 3; i++) {
        float invD = 1.0f / dir[i];
        float t0 = (bmin[i] - origin[i]) * invD;
        float t1 = (bmax[i] - origin[i]) * invD;
        if (invD < 0.0f) std::swap(t0, t1);
        tMin = std::max(tMin, t0);
        tMax = std::min(tMax, t1);
    }
    t = tMin;
    return tMax >= std::max(tMin, 0.0f);
}

void MainEditorScreen::handleModelClick(double mx, double my) {
    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);

    float ndcX = static_cast<float>(2.0 * mx / fbW - 1.0);
    float ndcY = static_cast<float>(1.0 - 2.0 * my / fbH);

    glm::mat4 invProjView = glm::inverse(
        camera.getProjectionMatrix(static_cast<float>(fbW) / static_cast<float>(fbH)) *
        camera.getViewMatrix()
    );

    glm::vec4 nearClip = invProjView * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 farClip  = invProjView * glm::vec4(ndcX, ndcY,  1.0f, 1.0f);
    nearClip /= nearClip.w;
    farClip  /= farClip.w;

    glm::vec3 rayOrigin = camera.position;
    glm::vec3 rayDir = glm::normalize(glm::vec3(farClip) - glm::vec3(nearClip));

    float closestT = INFINITY;
    MeshInstance3D* hitNode = nullptr;

    for (auto* meshNode : scene.GetAllMeshInstances()) {
        LoadedMesh* meshData = meshManager.GetMeshByPath(meshNode->meshAssetPath);
        if (!meshData) continue;

        glm::mat4 globalTransform = meshNode->GetGlobalTransform();

        glm::vec3 worldMin = glm::vec3(globalTransform * glm::vec4(meshData->bboxMin, 1.0f));
        glm::vec3 worldMax = glm::vec3(globalTransform * glm::vec4(meshData->bboxMax, 1.0f));

        float tHit;
        if (rayAABBIntersect(rayOrigin, rayDir,
                             glm::min(worldMin, worldMax),
                             glm::max(worldMin, worldMax),
                             tHit) && tHit < closestT) {
            closestT = tHit;
            hitNode = meshNode;
        }
    }

    for (auto* meshNode : scene.GetAllMeshInstances()) {
        meshNode->selected = (meshNode == hitNode);
    }
}

void MainEditorScreen::drawSelectionBox(MeshInstance3D* meshNode) {
    if (!gridShaderProgram || selectionVAO == 0) return;

    LoadedMesh* meshData = meshManager.GetMeshByPath(meshNode->meshAssetPath);
    if (!meshData) return;

    glm::vec3 bboxCenter = (meshData->bboxMin + meshData->bboxMax) * 0.5f;
    glm::vec3 bboxSize   = meshData->bboxMax - meshData->bboxMin;

    glm::mat4 boxModel = meshNode->GetGlobalTransform()
                        * glm::translate(glm::mat4(1.0f), bboxCenter)
                        * glm::scale(glm::mat4(1.0f), bboxSize);

    glm::mat4 view = camera.getViewMatrix();
    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    glm::mat4 proj = camera.getProjectionMatrix(static_cast<float>(fbW) / static_cast<float>(fbH));

    glUseProgram(gridShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(gridShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(boxModel));
    glUniformMatrix4fv(glGetUniformLocation(gridShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(gridShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(gridShaderProgram, "viewPos"), 1, glm::value_ptr(camera.position));
    glUniform1f(glGetUniformLocation(gridShaderProgram, "fadeNear"), -1.0f);
    glUniform1f(glGetUniformLocation(gridShaderProgram, "fadeFar"), 1e8f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glBindVertexArray(selectionVAO);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
