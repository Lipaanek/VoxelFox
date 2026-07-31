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