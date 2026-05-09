#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP

#include <string>

std::string loadFile(const char* path);
unsigned int compileShader(unsigned int type, const std::string& source);
unsigned int compileComputeShader(const char* path);

#endif