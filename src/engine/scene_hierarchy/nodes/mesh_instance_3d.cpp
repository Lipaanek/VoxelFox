#include "mesh_instance_3d.hpp"
#include "../../../include/loaded_mesh.hpp"
#include "../../../include/mesh.hpp"
#include "../../../include/voxel.hpp"
#include "../../../engine/mesh_manager.hpp"
#include <imgui/imgui.h>

void MeshInstance3D::SetMesh(const std::string& assetPath, MeshManager& meshManager) {
    meshAssetPath = assetPath;
    meshData_ = meshManager.GetMeshByPath(assetPath);
}

const Mesh* MeshInstance3D::GetRenderMesh() const {
    return meshData_ ? &meshData_->renderMesh : nullptr;
}

glm::vec3 MeshInstance3D::GetBBoxMin() const {
    return meshData_ ? meshData_->bboxMin : glm::vec3(0.0f);
}

glm::vec3 MeshInstance3D::GetBBoxMax() const {
    return meshData_ ? meshData_->bboxMax : glm::vec3(0.0f);
}

const std::vector<VoxelChunk>* MeshInstance3D::GetVoxelChunks() const {
    return meshData_ ? &meshData_->voxelChunks : nullptr;
}

void MeshInstance3D::BuildUI() {
    Node3D::BuildUI();

    ImGui::Separator();
    ImGui::Text("Mesh: %s", meshAssetPath.c_str());
    ImGui::Checkbox("Selected", &selected);

    if (meshData_) {
        glm::vec3 bmin = GetBBoxMin();
        glm::vec3 bmax = GetBBoxMax();
        ImGui::Text("BBox Min: (%.3f, %.3f, %.3f)", bmin.x, bmin.y, bmin.z);
        ImGui::Text("BBox Max: (%.3f, %.3f, %.3f)", bmax.x, bmax.y, bmax.z);
        ImGui::Text("Tris: %zu", meshData_->voxelChunks.size());
    }
}
