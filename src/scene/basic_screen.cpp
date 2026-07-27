#include "basic_screen.hpp"

void BasicScreen::render() {
    mesh.draw(this->programID);
}

void BasicScreen::onReady() {
    // Basic triangle setup
    std::vector<Vertex> verts = {
            {{ -0.5f, -0.5f, 0.0f }},
            {{ 0.5f, -0.5f, 0.0f }},
            {{ 0.0f, 0.5f, 0.0f }}
        };

    std::vector<GLuint> idxs = { 0, 1, 2 };

    mesh.setVertices(verts);
    mesh.setIndices(idxs);
    mesh.setup();
}