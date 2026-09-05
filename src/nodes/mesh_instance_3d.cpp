#include "mesh_instance_3d.hpp"

#include "core/scene/scene.hpp"

MeshInstance3D::MeshInstance3D() {
    this->setName("MeshInstance3D");
}

void MeshInstance3D::setMesh(MeshID mesh, const AABB &aabb) {
    this->mesh = mesh;

    BoundingSphere sphere {};
    sphere.center =
        (aabb.min + aabb.max) * 0.5f;

    sphere.radius =
        glm::length(aabb.max - sphere.center);

    this->sphere = sphere;
    this->transform.dirty = true;
}

BoundingSphere MeshInstance3D::getBoundingSphere() const {
    return this->sphere;
}

BoundingSphere MeshInstance3D::getGlobalBoundingSphere() const {
    if (this->transform.dirty) {
        const glm::mat4 model = getGlobalMatrix();
        m_globalSphere.center = glm::vec3(model * glm::vec4(sphere.center, 1.0f));
        const float scaleX = glm::length(glm::vec3(model[0]));
        const float scaleY = glm::length(glm::vec3(model[1]));
        const float scaleZ = glm::length(glm::vec3(model[2]));
        m_globalSphere.radius = sphere.radius * glm::max(scaleX, glm::max(scaleY, scaleZ));
        this->transform.dirty = false;
    }
    return m_globalSphere;
}

MeshID MeshInstance3D::getMesh() const {
    return this->mesh;
}

void MeshInstance3D::setColor(glm::vec3 color) {
    this->color = color;
}

glm::vec3 MeshInstance3D::getColor() const {
    return this->color;
}

MeshInstance3D::MeshInstance3D(const std::string &name) {
    this->setName(name);
}

void MeshInstance3D::onTreeEnter(Scene *newScene) {
    newScene->getChunkManager().registerNode(this);
}
