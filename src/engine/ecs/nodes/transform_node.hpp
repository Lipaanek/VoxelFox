#ifndef TRANSFORM_NODE_HPP
#define TRANSFORM_NODE_HPP

#include "../node.hpp"

class TransformNode : public Node {
    public:
        struct Transform {
            glm::vec3 position{0.0f};
            glm::vec3 rotation{0.0f};
            glm::vec3 scale{1.0f};
        };
    
        Transform transform;

        virtual std::vector<PropertyDescriptor> GetTransformProperties() const {
            return {
                {"Position", PropertyType::VEC3, &transform.position, {}, "Transform"},
                {"Rotation", PropertyType::VEC3, &transform.rotation, {0.0f, 360.0f}, "Transform"},
                {"Scale", PropertyType::VEC3, &transform.scale, {0.01f, 10.0f}, "Transform"}
            };
        }
};

#endif