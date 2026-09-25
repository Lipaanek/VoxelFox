#pragma once
#include <vector>

#include "core/renderer/frame_render_data.hpp"

struct PanelVertex {
    glm::vec2 position;
};

struct PanelData {
    ShaderProgram program;

    VertexArray vao;
    Buffer vbo { GL_ARRAY_BUFFER };
    Buffer ebo { GL_ELEMENT_ARRAY_BUFFER };

    Layout layout2D = {
        {
            { .location = 0, .size = 2, .type = GL_FLOAT, .offset = offsetof(PanelVertex, position) },
        },
        sizeof(PanelVertex)
    };
};

class UIRenderer {
private:
    PanelData panelRenderData;

public:
    UIRenderer();

    void setupPanelShaders();
    void render(const FrameRenderData& frameData);
};
