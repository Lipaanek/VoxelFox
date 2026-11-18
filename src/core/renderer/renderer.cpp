#include "renderer.hpp"

void Renderer::render(const FrameRenderData &frd, Scene& scene) {
    this->m_meshRenderer.render(frd, scene);
}
