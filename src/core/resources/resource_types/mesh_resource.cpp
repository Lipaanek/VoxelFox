#include "mesh_resource.hpp"

#include "../../model_loading/obj_loader.hpp"
#include "../../util/util.hpp"

bool MeshResource::setup(const MeshData& data) {
    if (data.vertices.empty() || data.indices.empty()) {
        Util::Log::error("Cannot initialize Mesh Resource without vertices or indices");
        return false;
    }

    this->m_vao.bind();
    this->m_vbo.upload(data.vertices.data(), data.vertices.size() * sizeof(Vertex));
    this->m_ebo.upload(data.indices.data(), data.indices.size() * sizeof(GLuint));

    this->m_layout3D.upload();
    this->m_indexCount = data.indices.size();
    return true;
}

bool MeshResource::loadFromFile(const ResourceLoadInfo& loadInfo) {
    const auto* data = dynamic_cast<const MeshResourceLoadInfo*>(&loadInfo);
    if (!data) {
        Util::Log::error("MeshResource received incompatible load info");
        return false;
    }

    if (data->meshData) {
        return setup(*data->meshData);
    }

    if (data->objectFile.empty()) {
        Util::Log::error("MeshResource requires mesh data or an object file");
        return false;
    }

    ObjLoader loader;
    const MeshData meshData = loader.load(
        data->objectFile,
        data->materialFile
    );

    const MeshAssetRefs refs {
        .objectFile = data->objectFile,
        .materialFile = data->materialFile
    };

    this->assetRefs = refs;
    this->path = data->objectFile;

    return setup(meshData);
}
