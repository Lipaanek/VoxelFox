#pragma once

#include <glad/glad.h>
#include <vector>

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
    void download(void* out, GLsizeiptr size) const;

    template<typename T>
    void read(std::vector<T>& out) const {
        bind();
        GLint size = 0;
        glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
        out.resize(static_cast<size_t>(size) / sizeof(T));
        download(out.data(), size);
    }

    [[nodiscard]] GLuint getID() const;
};