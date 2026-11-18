#include "vertex_array.hpp"
#include "../util/util.hpp"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &this->m_id);
    if (this->m_id == 0)
        Util::Log::error("Failed to generate vertex array");
}

VertexArray::~VertexArray() {
    if (m_id)
        glDeleteVertexArrays(1, &this->m_id);
}

VertexArray::VertexArray(VertexArray&& other) noexcept : m_id(other.m_id) {
    other.m_id = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    if (this != &other) {
        if (m_id)
            glDeleteVertexArrays(1, &this->m_id);
        m_id = other.m_id;
        other.m_id = 0;
    }
    return *this;
}

void VertexArray::bind() const {
    glBindVertexArray(this->m_id);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

GLuint VertexArray::getID() const {
    return m_id;
}
