#ifndef VERTEX_HPP
#define VERTEX_HPP
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord) 
        : position(pos), normal(norm), uv(texCoord) {}
};

#endif