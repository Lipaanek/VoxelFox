#include "shader.hpp"
#include "../util/util.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& path, ShaderType shaderType)
    : path(path), shaderType(shaderType) {}

GLuint Shader::compile() {
    std::string src = Util::File::read(this->path);
    if (src.empty()) {
        Util::Log::error("Source code of shader is empty.");
        return 0;
    }

    // Create shader object
    GLuint shader = glCreateShader(toGL(this->shaderType));
    if (shader == 0) {
        Util::Log::error("Failed to create shader.");
        return 0;
    }

    // Bind source to shader compilation
    const char* source = src.c_str();
    glShaderSource(shader, 1, &source, nullptr);

    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        // Log error
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        Util::Log::error(std::string("Shader compile error: ") + infoLog);

        // Need to delete shader to avoid leak
        glDeleteShader(shader);
        return 0;
    }

    this->id = shader;

    return shader;
}