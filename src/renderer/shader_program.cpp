#include "shader_program.hpp"
#include "../util/util.hpp"
#include <format>

ShaderProgram::ShaderProgram() {
    GLuint program = glCreateProgram();
    if (!program) {
        Util::Log::error("Failed to create shader program");
        return;
    }

    this->id = program;
}

void ShaderProgram::attach(const Shader& shader) {
    this->shaders.push_back(&shader);

    glAttachShader(this->id, shader.getID());
}

void ShaderProgram::link() {
    glLinkProgram(this->id);

    GLuint success;
    glGetProgramiv(this->id, GL_LINK_STATUS, (int *)&success);

    if (!success) {
        GLint maxLength = 0;
	    glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &maxLength);

	    // The maxLength includes the NULL character
	    std::vector<GLchar> infoLog(maxLength);
	    glGetProgramInfoLog(this->id, maxLength, &maxLength, &infoLog[0]);

        Util::Log::error(std::format("Shader linking failed: {}", infoLog.data()));

	    // Don't need the program anymore
	    glDeleteProgram(this->id);
        this->id = 0;

        return;
    }

    // const Shader*
    for (auto shader : this->shaders) {
        glDetachShader(this->id, shader->getID());
    }
}