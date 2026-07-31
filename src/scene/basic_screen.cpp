#include "basic_screen.hpp"
#include <glm/gtc/matrix_transform.hpp>

void BasicScreen::render() {
    float time = static_cast<float>(glfwGetTime());

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    this->program.setUniform("u_model", model);
    this->program.setUniform("u_view", this->camera.getViewMatrix());
    this->program.setUniform("u_projection", this->camera.getProjectionMatrix(this->window.getAspect()));

    mesh.draw(this->program);
}

void BasicScreen::onReady() {
    // Cube with per-face colors
    std::vector<Vertex> verts = {
            // Back face (z = -0.5)
            {{ -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
            {{  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
            {{  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
            {{ -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
            // Front face (z = 0.5)
            {{ -0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            {{ -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
            // Left face (x = -0.5)
            {{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }},
            {{ -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }},
            {{ -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }},
            {{ -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f }},
            // Right face (x = 0.5)
            {{  0.5f, -0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f }},
            {{  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f }},
            {{  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f }},
            {{  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f }},
            // Bottom face (y = -0.5)
            {{ -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f }},
            {{  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f }},
            {{  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f }},
            {{ -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f }},
            // Top face (y = 0.5)
            {{ -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f }},
            {{  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f }},
            {{  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f }},
            {{ -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f }},
    };

    std::vector<GLuint> idxs = {
            // Back
            0, 1, 2,  2, 3, 0,
            // Front
            4, 5, 6,  6, 7, 4,
            // Left
            8, 9, 10,  10, 11, 8,
            // Right
            12, 13, 14,  14, 15, 12,
            // Bottom
            16, 17, 18,  18, 19, 16,
            // Top
            20, 21, 22,  22, 23, 20,
    };

    mesh.setVertices(verts);
    mesh.setIndices(idxs);
    mesh.setup();
}
