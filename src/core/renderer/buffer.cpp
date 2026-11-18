#include "buffer.hpp"
#include "../util/util.hpp"

Buffer::Buffer(BufferType target) : m_target(static_cast<GLenum>(target)) {
    glGenBuffers(1, &this->m_id);

    if (this->m_id == 0)
        Util::Log::error("Failed to generate buffer");
}

Buffer::~Buffer() {
    if (m_id)
        glDeleteBuffers(1, &this->m_id);
}

Buffer::Buffer(Buffer&& other) noexcept : m_target(other.m_target), m_id(other.m_id) {
    other.m_id = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        if (m_id)
            glDeleteBuffers(1, &this->m_id);
        m_target = other.m_target;
        m_id = other.m_id;
        other.m_id = 0;
    }
    return *this;
}

void Buffer::bind() const {
    glBindBuffer(this->m_target, this->m_id);
}

void Buffer::upload(const void* data, GLsizeiptr size, GLenum usage) const {
    this->bind();

    glBufferData(this->m_target, size, data, usage);
}

void Buffer::download(void* out, GLsizeiptr size) const {
    this->bind();

    glGetBufferSubData(this->m_target, 0, size, out);
}

GLuint Buffer::getID() const {
    return this->m_id;
}