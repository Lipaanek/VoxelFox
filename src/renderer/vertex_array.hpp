#pragma once

#include <glad/glad.h>

class VertexArray {
private:
    GLuint id = 0;

public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void bind() const;
    void unbind() const;

    [[nodiscard]] GLuint getID() const;
};
