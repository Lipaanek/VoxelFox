#pragma once

#include "screen.hpp"
#include <glad/glad.h>
#include "../renderer/mesh/mesh.hpp"

class BasicScreen : public Screen {
private:
    GLuint programID;
    Mesh mesh;

public:
    BasicScreen(GLuint programID) : programID(programID) {}

    void render() override;
    void onReady() override;
};