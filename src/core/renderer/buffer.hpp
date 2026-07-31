#pragma once

#include <glad/glad.h>

class Buffer {
private:
    GLenum target;
    GLuint id = 0;

public:
    explicit Buffer(GLenum target); // glGenBuffers
    ~Buffer();                      // glDeleteBuffers

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    void bind() const;
    void upload(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW) const;

    [[nodiscard]] GLuint getID() const;
};