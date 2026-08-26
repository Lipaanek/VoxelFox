#include "shader_program.hpp"
#include "../util/util.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <format>

ShaderProgram::ShaderProgram() {
    const GLuint program = glCreateProgram();
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

void ShaderProgram::use() const {
    if (id)
        glUseProgram(id);
}

void ShaderProgram::setStorageBuffer(const unsigned int binding, const Buffer& buffer) const {
    use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, buffer.getID());
}

void ShaderProgram::dispatch(unsigned int x, unsigned int y, unsigned int z) const {
    use();
    glDispatchCompute(x, y, z);
}

GLint ShaderProgram::getUniformLocation(const std::string& name) {
    auto it = this->uniformLocations.find(name);
    if (it != this->uniformLocations.end())
        return it->second;

    GLint location = glGetUniformLocation(this->id, name.c_str());
    this->uniformLocations[name] = location;

    if (location == -1)
        Util::Log::log(std::format("Uniform '{}' not found in shader program", name));

    return location;
}

void ShaderProgram::setUniform(const std::string& name, bool value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform1i(location, value ? 1 : 0);
}

void ShaderProgram::setUniform(const std::string& name, int value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform1i(location, value);
}

void ShaderProgram::setUniform(const std::string& name, unsigned int value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform1ui(location, value);
}

void ShaderProgram::setUniform(const std::string& name, float value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec2& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform2f(location, value.x, value.y);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform3f(location, value.x, value.y, value.z);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat2& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat2x3& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat2x4& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3x2& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3x4& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4x2& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4x3& value) {
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const std::vector<int>& value) {
    if (value.empty()) return;
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform1iv(location, static_cast<GLsizei>(value.size()), value.data());
}

void ShaderProgram::setUniform(const std::string& name, const std::vector<float>& value) {
    if (value.empty()) return;
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform1fv(location, static_cast<GLsizei>(value.size()), value.data());
}

void ShaderProgram::setUniform(const std::string& name, const std::vector<glm::vec2>& value) {
    if (value.empty()) return;
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform2fv(location, static_cast<GLsizei>(value.size()), glm::value_ptr(value[0]));
}

void ShaderProgram::setUniform(const std::string& name, const std::vector<glm::vec3>& value) {
    if (value.empty()) return;
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform3fv(location, static_cast<GLsizei>(value.size()), glm::value_ptr(value[0]));
}

void ShaderProgram::setUniform(const std::string& name, const std::vector<glm::vec4>& value) {
    if (value.empty()) return;
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniform4fv(location, static_cast<GLsizei>(value.size()), glm::value_ptr(value[0]));
}

void ShaderProgram::setUniform(const std::string& name, const std::vector<glm::mat2>& value) {
    if (value.empty()) return;
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix2fv(location, static_cast<GLsizei>(value.size()), GL_FALSE, glm::value_ptr(value[0]));
}

void ShaderProgram::setUniform(const std::string& name, const std::vector<glm::mat3>& value) {
    if (value.empty()) return;
    GLint location = getUniformLocation(name);
    if (location == -1) return;

    use();
    glUniformMatrix3fv(location, static_cast<GLsizei>(value.size()), GL_FALSE, glm::value_ptr(value[0]));
}

void ShaderProgram::setUniform(const std::string& name, const std::vector<glm::mat4>& value) {
    if (value.empty()) return;
    GLint location = getUniformLocation(name);
    if (location == -1) return;
    
    use();
    glUniformMatrix4fv(location, static_cast<GLsizei>(value.size()), GL_FALSE, glm::value_ptr(value[0]));
}