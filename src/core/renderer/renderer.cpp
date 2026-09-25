#include "renderer.hpp"

void Renderer::render(const FrameRenderData &frd, Scene& scene) {
    this->meshRenderer.render(frd, scene);
    this->uiRenderer.render(frd);
}
