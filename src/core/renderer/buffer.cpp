#include "buffer.hpp"
#include "../util/util.hpp"

Buffer::Buffer(GLenum target) : target(target) {
    glGenBuffers(1, &this->id);

    if (this->id == 0)
        Util::Log::error("Failed to generate buffer");
}

Buffer::~Buffer() {
    if (id)
        glDeleteBuffers(1, &this->id);
}

Buffer::Buffer(Buffer&& other) noexcept : target(other.target), id(other.id) {
    other.id = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        if (id)
            glDeleteBuffers(1, &this->id);
        target = other.target;
        id = other.id;
        other.id = 0;
    }
    return *this;
}

void Buffer::bind() const {
    glBindBuffer(this->target, this->id);
}

void Buffer::upload(const void* data, GLsizeiptr size, GLenum usage) const {
    this->bind();

    glBufferData(this->target, size, data, usage);
}

GLuint Buffer::getID() const {
    return this->id;
}