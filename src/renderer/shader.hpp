#pragma once

#include <string>
#include <glad/glad.h>

enum class ShaderType : unsigned int {
    Vertex          = GL_VERTEX_SHADER,
    Fragment        = GL_FRAGMENT_SHADER,
    Geometry        = GL_GEOMETRY_SHADER,
    TessControl     = GL_TESS_CONTROL_SHADER,
    TessEvaluation  = GL_TESS_EVALUATION_SHADER,
    Compute         = GL_COMPUTE_SHADER
};

constexpr unsigned int toGL(ShaderType type) {
    return static_cast<unsigned int>(type);
}

class Shader {
private:
    std::string path;
    ShaderType shaderType;
    GLuint id;

public:
    Shader(const std::string& path, ShaderType shaderType);
    ~Shader() {
        if (id)
            glDeleteShader(id);
    }

    unsigned int compile();
};