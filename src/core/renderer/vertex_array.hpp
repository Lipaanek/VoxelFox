#pragma once

#include <glad/glad.h>

class VertexArray {
private:
    GLuint id = 0;

public:
    VertexArray(); // glGenVertexArrays
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void bind() const;
    void unbind() const;

    [[nodiscard]] GLuint getID() const;
};
