#ifndef OBJECT_LOADER_HPP
#define OBJECT_LOADER_HPP

#include <string>

#include "mesh.hpp"
#include "vertex.hpp"

using namespace std;

vector<Vertex> getWorldData();
void loadObject(string path);

#endif