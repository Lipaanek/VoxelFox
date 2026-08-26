#include "ui_renderer.hpp"

#include "../../../thirdparty/glad/glad.h"

void UIRenderer::drawPanel(Panel &panel) {
    this->shaderProgram.use();

    this->vao.bind();

    //glDrawElements(GL_TRIANGLES, );
}
