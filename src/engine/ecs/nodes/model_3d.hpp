#ifndef MODEL_3D_HPP
#define MODEL_3D_HPP

#include "../../include/loaded_mesh.hpp"
#include "../node.hpp"

using Entity = uint32_t;
constexpr Entity INVALID_ENTITY = 0;

class Model3D : Node {
    public:
        Model3D {
            name = "Model3D";
        }
};

#endif