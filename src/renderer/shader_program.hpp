#pragma once

#include "shader.hpp"
#include <vector>

class ShaderProgram {
private:
    GLuint id = 0;
    std::vector<const Shader*> shaders;

public:
    ShaderProgram();
    ~ShaderProgram() {
        if (id)
            glDeleteProgram(id);
    }

    void attach(const Shader& shader);
    void link();

    [[nodiscard]] GLuint getID() const {
        return id;
    }
};