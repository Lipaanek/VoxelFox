#pragma once

#include "panel.hpp"
#include "../renderer/buffer.hpp"
#include "../renderer/shader_program.hpp"
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "../renderer/vertex_array.hpp"

class UIRenderer {
private:
    VertexArray vao;
    Buffer vbo { GL_ARRAY_BUFFER };
    ShaderProgram shaderProgram;

    glm::mat4 projection;

public:
    void drawPanel(Panel& panel);
};
