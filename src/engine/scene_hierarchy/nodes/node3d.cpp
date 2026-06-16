#include "node3d.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui/imgui.h>

static glm::quat RotationBetweenVectors(const glm::vec3& from, const glm::vec3& to) {
    float dot = glm::dot(from, to);
    if (dot > 0.99999f) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    if (dot < -0.99999f) {
        glm::vec3 cross = glm::cross(from, glm::vec3(1.0f, 0.0f, 0.0f));
        if (glm::length(cross) < 0.0001f)
            cross = glm::cross(from, glm::vec3(0.0f, 1.0f, 0.0f));
        return glm::angleAxis(glm::pi<float>(), glm::normalize(cross));
    }
    glm::vec3 axis = glm::cross(from, to);
    float angle = acos(dot);
    return glm::angleAxis(angle, glm::normalize(axis));
}

void Node3D::SetPosition(const glm::vec3& pos) {
    position = pos;
    transformDirty = true;
}

void Node3D::Translate(const glm::vec3& delta) {
    position += delta;
    transformDirty = true;
}

void Node3D::SetScale(const glm::vec3& scl) {
    scale = scl;
    transformDirty = true;
}

void Node3D::Rotate(const glm::vec3& axis, float angleDeg) {
    glm::quat q = glm::angleAxis(glm::radians(angleDeg), glm::normalize(axis));
    SetQuaternion(q * GetQuaternion());
}

void Node3D::LookAt(const glm::vec3& target) {
    glm::vec3 direction = target - position;
    float len = glm::length(direction);
    if (len < 0.0001f) return;
    direction /= len;

    glm::quat q = RotationBetweenVectors(glm::vec3(0.0f, 0.0f, -1.0f), direction);
    SetQuaternion(q);
}

glm::quat Node3D::GetQuaternion() const {
    return glm::quat(glm::radians(eulerRotation));
}

void Node3D::SetQuaternion(const glm::quat& q) {
    eulerRotation = glm::degrees(glm::eulerAngles(q));
    transformDirty = true;
}

glm::mat4 Node3D::GetLocalTransform() const {
    if (transformDirty) {
        UpdateTransform();
    }
    return localTransform;
}

glm::mat4 Node3D::GetGlobalTransform() const {
    glm::mat4 local = GetLocalTransform();
    if (parent) {
        Node3D* parent3D = dynamic_cast<Node3D*>(parent);
        if (parent3D) {
            return parent3D->GetGlobalTransform() * local;
        }
    }
    return local;
}

void Node3D::UpdateTransform() const {
    glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 r = glm::mat4_cast(GetQuaternion());
    glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
    localTransform = t * r * s;
    transformDirty = false;
}

void Node3D::BuildUI() {
    float pos[3] = {position.x, position.y, position.z};
    if (ImGui::DragFloat3("Position", pos, 0.1f)) {
        SetPosition(glm::vec3(pos[0], pos[1], pos[2]));
    }

    float rot[3] = {eulerRotation.x, eulerRotation.y, eulerRotation.z};
    if (ImGui::DragFloat3("Rotation", rot, 1.0f)) {
        eulerRotation = glm::vec3(rot[0], rot[1], rot[2]);
        transformDirty = true;
    }

    float scl[3] = {scale.x, scale.y, scale.z};
    if (ImGui::DragFloat3("Scale", scl, 0.1f)) {
        SetScale(glm::vec3(scl[0], scl[1], scl[2]));
    }
}
