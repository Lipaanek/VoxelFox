#include "basic_screen.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../core/model_loading/obj_loader.hpp"

void BasicScreen::render() {
    float time = static_cast<float>(glfwGetTime());

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    this->program.setUniform("u_model", model);
    this->program.setUniform("u_view", this->camera.getViewMatrix());
    this->program.setUniform("u_projection", this->camera.getProjectionMatrix(this->window.getAspect()));
    this->lights.uploadLights(this->program);

    mesh.draw(this->program);
}

void BasicScreen::onReady() {
    ObjLoader loader;
    MeshData meshData = loader.Load(
        "C:\\Users\\lipov\\Downloads\\nl9ixa6iwyrk-WoodenLarry\\WoodenLarry.obj",
        "C:\\Users\\lipov\\Downloads\\nl9ixa6iwyrk-WoodenLarry\\WoodenLarry.mtl"
    );

    Light point;
    point.type = LightType::Point;
    point.position = glm::vec3(2.0f, 1.0f, 2.0f);
    point.color = glm::vec3(1.0f, 1.0f, 1.0f);
    point.intensity = 3.0f;
    this->lights.addLight(point);


    // Cube with per-face colors
    // glm::vec3 backNormal(0.0f, 0.0f, -1.0f);
    // glm::vec3 frontNormal(0.0f, 0.0f, 1.0f);
    // glm::vec3 leftNormal(-1.0f, 0.0f, 0.0f);
    // glm::vec3 rightNormal(1.0f, 0.0f, 0.0f);
    // glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
    // glm::vec3 topNormal(0.0f, 1.0f, 0.0f);

    // std::vector<Vertex> verts = {
    //         // Back face (z = -0.5)
    //         { { -0.5f, -0.5f, -0.5f }, backNormal,  { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    //         { {  0.5f, -0.5f, -0.5f }, backNormal,  { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    //         { {  0.5f,  0.5f, -0.5f }, backNormal,  { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
    //         { { -0.5f,  0.5f, -0.5f }, backNormal,  { 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },
    //         // Front face (z = 0.5)
    //         { { -0.5f, -0.5f,  0.5f }, frontNormal, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    //         { {  0.5f, -0.5f,  0.5f }, frontNormal, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    //         { {  0.5f,  0.5f,  0.5f }, frontNormal, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
    //         { { -0.5f,  0.5f,  0.5f }, frontNormal, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
    //         // Left face (x = -0.5)
    //         { { -0.5f, -0.5f, -0.5f }, leftNormal,  { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    //         { { -0.5f, -0.5f,  0.5f }, leftNormal,  { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    //         { { -0.5f,  0.5f,  0.5f }, leftNormal,  { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
    //         { { -0.5f,  0.5f, -0.5f }, leftNormal,  { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
    //         // Right face (x = 0.5)
    //         { {  0.5f, -0.5f,  0.5f }, rightNormal, { 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } },
    //         { {  0.5f, -0.5f, -0.5f }, rightNormal, { 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } },
    //         { {  0.5f,  0.5f, -0.5f }, rightNormal, { 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } },
    //         { {  0.5f,  0.5f,  0.5f }, rightNormal, { 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } },
    //         // Bottom face (y = -0.5)
    //         { { -0.5f, -0.5f,  0.5f }, bottomNormal, { 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f } },
    //         { {  0.5f, -0.5f,  0.5f }, bottomNormal, { 1.0f, 0.0f }, { 1.0f, 0.0f, 1.0f } },
    //         { {  0.5f, -0.5f, -0.5f }, bottomNormal, { 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } },
    //         { { -0.5f, -0.5f, -0.5f }, bottomNormal, { 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } },
    //         // Top face (y = 0.5)
    //         { { -0.5f,  0.5f, -0.5f }, topNormal, { 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    //         { {  0.5f,  0.5f, -0.5f }, topNormal, { 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    //         { {  0.5f,  0.5f,  0.5f }, topNormal, { 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } },
    //         { { -0.5f,  0.5f,  0.5f }, topNormal, { 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } },
    // };

    // std::vector<GLuint> idxs = {
    //         // Back
    //         0, 1, 2,  2, 3, 0,
    //         // Front
    //         4, 5, 6,  6, 7, 4,
    //         // Left
    //         8, 9, 10,  10, 11, 8,
    //         // Right
    //         12, 13, 14,  14, 15, 12,
    //         // Bottom
    //         16, 17, 18,  18, 19, 16,
    //         // Top
    //         20, 21, 22,  22, 23, 20,
    // };

    mesh.setVertices(meshData.vertices);
    mesh.setIndices(meshData.indices);
    mesh.setup();
}
