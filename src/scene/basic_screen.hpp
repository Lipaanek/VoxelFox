#pragma once

#include "../core/screen/screen.hpp"
#include <glad/glad.h>
#include "../core/renderer/mesh/mesh.hpp"

class BasicScreen : public Screen {
private:
    ShaderProgram& program;
    Mesh mesh;

public:
    BasicScreen(ShaderProgram& program) : program(program) {}

    void render() override;
    void onReady() override;
};