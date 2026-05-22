#ifndef MODEL_3D_HPP
#define MODEL_3D_HPP

#include "transform_node.hpp"
#include "../../include/loaded_mesh.hpp"
#include "../node_type.hpp"
#include "../property_system.hpp"

class Model3D : TransformNode {
    public:
        struct ModelData {
            LoadedMesh* mesh = nullptr;
            bool visible = true;
        };

        ModelData data;

        std::vector<PropertyDescriptor> GetProperties() const override {
            auto props = GetTransformProperties();
            // !Note: mesh pointer is managed externally by MeshManager
            // !Could add mesh name/path as a string property for editor UI
            props.push_back({"Visible", PropertyType::BOOL, &model.visible, {}, "Model"});
            return props;
        }

        NodeType GetType() const override {
            return NodeType::MODEL_3D;
        }

        const char* GetName() const override {
            return "Model3D";
        }
}

#endif