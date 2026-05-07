using namespace std;

#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP

std::string loadFile(const char* path);
unsigned int compileShader(unsigned int type, const std::string& source);

#endif