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

VertexArray::VertexArray(VertexArray&& other) noexcept : id(other.id) {
    other.id = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    if (this != &other) {
        if (id)
            glDeleteVertexArrays(1, &this->id);
        id = other.id;
        other.id = 0;
    }
    return *this;
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
