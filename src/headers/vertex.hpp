#ifndef VERTEX_HPP
#define VERTEX_HPP
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec4 position; // Use vec4 for GPU alignment (w is ignored)
    glm::vec4 normal;   // Use vec4 for GPU alignment (w is ignored)
    glm::vec2 uv;
    glm::vec2 padding;  // Keep the struct a multiple of 16 bytes for std430

    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord)
        : position(glm::vec4(pos, 0.0f)), normal(glm::vec4(norm, 0.0f)), uv(texCoord), padding(0.0f) {}

    Vertex(glm::vec4 pos, glm::vec4 norm, glm::vec2 texCoord, glm::vec2 pad = glm::vec2(0.0f))
        : position(pos), normal(norm), uv(texCoord), padding(pad) {}
};

#endif