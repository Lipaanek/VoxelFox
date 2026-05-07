#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../headers/shader_loader.hpp"

using namespace std;

string loadFile(const char* path)
{
    ifstream file(path, ios::in);
    if (!file.is_open())
    {
        // Log failed file opening
        printf("Failed to open file: %s \n", path);
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf(); // write to buffer
    return buffer.str(); // return file contents
}

unsigned int compileShader(unsigned int type, const string& source)
{
    unsigned int shader = glCreateShader(type);

    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        printf("Shader compile error: %s \n", infoLog);
    }

    return shader;
}