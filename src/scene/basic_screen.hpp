#pragma once

#include "screen.hpp"
#include <glad/glad.h>
#include "../renderer/mesh/mesh.hpp"

class BasicScreen : public Screen {
private:
    ShaderProgram& program;
    Mesh mesh;

public:
    BasicScreen(ShaderProgram& program) : program(program) {}

    void render() override;
    void onReady() override;
};