#ifndef VOXEL_HPP
#define VOXEL_HPP

#include <glm/glm.hpp>

using namespace std;

struct Voxel {
    glm::ivec3 position;
    bool solid;
};

#endif