#pragma once

#include "shader.hpp"
#include "buffer.hpp"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class ShaderProgram {
private:
    GLuint id = 0;
    std::vector<const Shader*> shaders;
    std::unordered_map<std::string, GLint> uniformLocations;

    [[nodiscard]] GLint getUniformLocation(const std::string& name);

public:
    ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ~ShaderProgram() {
        if (id)
            glDeleteProgram(id);
    }

    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void attach(const Shader& shader);
    void link();
    void use() const;

    void setStorageBuffer(unsigned int binding, const Buffer& buffer) const;
    void dispatch(unsigned int x, unsigned int y = 1, unsigned int z = 1) const;

    [[nodiscard]] GLuint getID() const {
        return id;
    }

    // Overloads for uniforms
    void setUniform(const std::string& name, bool value);
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, unsigned int value);
    void setUniform(const std::string& name, float value);

    void setUniform(const std::string& name, const glm::vec2& value);
    void setUniform(const std::string& name, const glm::vec3& value);
    void setUniform(const std::string& name, const glm::vec4& value);

    void setUniform(const std::string& name, const glm::mat2& value);
    void setUniform(const std::string& name, const glm::mat3& value);
    void setUniform(const std::string& name, const glm::mat4& value);

    void setUniform(const std::string& name, const glm::mat2x3& value);
    void setUniform(const std::string& name, const glm::mat2x4& value);
    void setUniform(const std::string& name, const glm::mat3x2& value);
    void setUniform(const std::string& name, const glm::mat3x4& value);
    void setUniform(const std::string& name, const glm::mat4x2& value);
    void setUniform(const std::string& name, const glm::mat4x3& value);

    void setUniform(const std::string& name, const std::vector<int>& value);
    void setUniform(const std::string& name, const std::vector<float>& value);
    void setUniform(const std::string& name, const std::vector<glm::vec2>& value);
    void setUniform(const std::string& name, const std::vector<glm::vec3>& value);
    void setUniform(const std::string& name, const std::vector<glm::vec4>& value);
    void setUniform(const std::string& name, const std::vector<glm::mat2>& value);
    void setUniform(const std::string& name, const std::vector<glm::mat3>& value);
    void setUniform(const std::string& name, const std::vector<glm::mat4>& value);
};
