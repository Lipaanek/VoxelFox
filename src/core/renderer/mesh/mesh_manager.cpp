#include "mesh_manager.hpp"
#include "../../util/util.hpp"

#include <utility>

unsigned int MeshManager::add(const MeshData& mesh) {
    Mesh owned;
    owned.setData(mesh);
    owned.setup();

    unsigned int uid = this->nextUID++;
    this->meshes.emplace(uid, std::move(owned));

    return uid;
}

void MeshManager::update(const MeshID uid, const MeshData &data) {
    const auto it = this->meshes.find(uid);
    if (it == this->meshes.end()) {
        Util::Log::error("SceneMeshManager: update() called for unregistered UID");
        return;
    }

    // Set mesh data and set it up
    it->second.setData(data);
    it->second.setup();
}

Mesh& MeshManager::get(const MeshID uid) {
    return this->meshes.at(uid);
}

void MeshManager::remove(const MeshID uid) {
    this->meshes.erase(uid);
}

void MeshManager::clear() {
    this->meshes.clear();
}

size_t MeshManager::getSize() const {
    return this->meshes.size();
}
