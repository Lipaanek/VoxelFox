#include "vertex_array.hpp"
#include "../util/util.hpp"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &this->id);
    if (this->id == 0)
        Util::Log::error("Failed to generate vertex array");
}

VertexArray::~VertexArray() {
    if (id)
        glDeleteVertexArrays(1, &this->id);
}

void VertexArray::bind() const {
    glBindVertexArray(this->id);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

GLuint VertexArray::getID() const {
    return id;
}
