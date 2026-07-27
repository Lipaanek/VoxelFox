#include "screen.hpp"

#include <glad/glad.h>

class BasicScreen : public Screen {
private:
    GLuint programID;

public:
    BasicScreen(GLuint programID) : programID(programID) {}

    void render() override {
        glUseProgram(this->programID);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};