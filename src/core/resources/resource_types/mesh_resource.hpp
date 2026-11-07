#pragma once
#include "../resource.hpp"
#include "../../../../thirdparty/glad/glad.h"
#include "../../renderer/buffer.hpp"
#include "../../renderer/layout.hpp"
#include "../../renderer/vertex_array.hpp"
#include "../../renderer/mesh/mesh_data.hpp"
#include "../../renderer/mesh/vertex.hpp"

struct MeshResourceLoadInfo : ResourceLoadInfo {
    std::string objectFile;
    std::string materialFile;
    const MeshData* meshData = nullptr;
};

struct MeshAssetRefs : AssetRefs {
    std::string objectFile;
    std::string materialFile;
};

class MeshResource : public Resource {
private:
    VertexArray vao;
    Buffer vbo { BufferType::ArrayBuffer };
    Buffer ebo { BufferType::ElementBuffer };

    Layout layout3D = {
        {
            { .location = 0, .size = 3, .type = GL_FLOAT, .offset = offsetof(Vertex, position)},
            { .location = 1, .size = 3, .type = GL_FLOAT, .offset = offsetof(Vertex, normal)},
            { .location = 2, .size = 2, .type = GL_FLOAT, .offset = offsetof(Vertex, texCoord)},
            { .location = 3, .size = 3, .type = GL_FLOAT, .offset = offsetof(Vertex, color)},
        }, sizeof(Vertex)
    };

    GLsizei indexCount = 0;

public:
    MeshResource() = default;

    bool setup(const MeshData &data);
    bool loadFromFile(const ResourceLoadInfo& loadInfo) override;

    Buffer& getVBO() { return vbo; }
    Buffer& getEBO() { return ebo; }
    VertexArray& getVAO() { return vao; }
    [[nodiscard]] GLsizei getIndexCount() const { return indexCount; }
};
