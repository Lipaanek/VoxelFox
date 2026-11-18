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
    std::vector<VertexAttribute> m_attributes;
    size_t m_stride;

public:
    Layout(std::initializer_list<VertexAttribute> attrs, size_t stride)
        : m_attributes(attrs), m_stride(stride) {}

    void upload() const {
        for (auto& a : this->m_attributes)
            a.upload(m_stride);
    }
};
