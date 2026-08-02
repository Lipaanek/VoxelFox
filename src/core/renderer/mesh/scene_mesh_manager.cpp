#include "scene_mesh_manager.hpp"
#include "../../util/util.hpp"

#include <utility>

unsigned int SceneMeshManager::add(const MeshData& mesh, const glm::mat4& model) {
    Mesh owned;
    owned.setData(mesh);
    owned.setup();

    unsigned int uid = this->nextUID++;
    this->meshes.emplace(uid, MeshEntry{ std::move(owned), model });

    return uid;
}

void SceneMeshManager::update(unsigned int uid, const MeshData& mesh) {
    auto it = this->meshes.find(uid);
    if (it == this->meshes.end()) {
        Util::Log::error("SceneMeshManager: update() called for unregistered UID");
        return;
    }

    it->second.mesh.setData(mesh);
    it->second.mesh.setup();
}

Mesh& SceneMeshManager::get(unsigned int uid) {
    return this->meshes.at(uid).mesh;
}

void SceneMeshManager::setTransform(unsigned int uid, const glm::mat4 model) {
    auto it = this->meshes.find(uid);
    if (it == this->meshes.end()) {
        Util::Log::error("SceneMeshManager: setTransform() called for unregistered UID");
        return;
    }

    it->second.model = model;
}

glm::mat4 SceneMeshManager::getTransform(unsigned int uid) const {
    return this->meshes.at(uid).model;
}

void SceneMeshManager::remove(unsigned int uid) {
    this->meshes.erase(uid);
}

void SceneMeshManager::clear() {
    this->meshes.clear();
}

void SceneMeshManager::render(ShaderProgram& program) {
    for (auto& [uid, entry] : this->meshes) {
        program.setUniform("u_model", entry.model);
        entry.mesh.draw(program);
    }
}
