#include "ui_renderer.hpp"

#include "core/util/util.hpp"

static constexpr PanelVertex panelVertices[] = {
    {{0.0f, 0.0f}},
    {{1.0f, 0.0f}},
    {{1.0f, 1.0f}},
    {{0.0f, 1.0f}}
};

static constexpr GLuint panelIndices[] = {
    0, 1, 2,
    2, 3, 0
};

void UIRenderer::setupPanelShaders() {
    Shader frag{
        "assets/shaders/ui/ui_panel.frag",
        ShaderType::Fragment
    };

    Shader vert{
        "assets/shaders/ui/ui_panel.vert",
        ShaderType::Vertex
    };

    frag.compile();
    vert.compile();

    if (frag.getID() == 0 || vert.getID() == 0) {
        Util::Log::error("Failed to compile UI panel shaders.");
        return;
    }

    panelRenderData.program.attach(vert);
    panelRenderData.program.attach(frag);
    panelRenderData.program.link();

    panelRenderData.vao.bind();

    panelRenderData.vbo.upload(
        panelVertices,
        sizeof(panelVertices),
        GL_STATIC_DRAW
    );

    panelRenderData.ebo.upload(
        panelIndices,
        sizeof(panelIndices),
        GL_STATIC_DRAW
    );

    panelRenderData.layout2D.upload();
}

UIRenderer::UIRenderer() {
    this->setupPanelShaders();
}

void UIRenderer::render(const FrameRenderData& frameData) {
    panelRenderData.program.use();
    panelRenderData.vao.bind();

    const GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    for (const auto&[transform, color, uiType] : frameData.gui) {
        if (uiType != UIType::Panel)
            continue;

        panelRenderData.program.setUniform(
            "u_position",
            transform.position
        );

        panelRenderData.program.setUniform(
            "u_size",
            transform.dimensions
        );

        panelRenderData.program.setUniform(
            "u_anchor",
            transform.anchor
        );

        panelRenderData.program.setUniform(
            "u_screenSize",
            glm::vec2(frameData.windowData.width, frameData.windowData.height)
        );

        panelRenderData.program.setUniform(
            "u_color",
            color
        );

        glDrawElements(
            GL_TRIANGLES,
            6,
            GL_UNSIGNED_INT,
            nullptr
        );
    }

    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
}
