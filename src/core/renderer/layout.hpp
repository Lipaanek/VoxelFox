#pragma once

#include <vector>
#include <glad/glad.h>

struct VertexAttribute {
    GLuint location;
    GLint size;
    GLenum type;
    size_t offset;

    void upload(const size_t stride) const {
        glEnableVertexAttribArray(this->location);
        glVertexAttribPointer(this->location, this->size, this->type, GL_FALSE, stride, reinterpret_cast<void *>(this->offset));
    }
};

class Layout {
private:
    std::vector<VertexAttribute> attributes;
    size_t stride;

public:
    Layout(std::initializer_list<VertexAttribute> attrs, size_t stride)
        : attributes(attrs), stride(stride) {}

    void upload() const {
        for (auto& a : this->attributes)
            a.upload(stride);
    }
};
