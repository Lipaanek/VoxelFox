#include "basic_scene.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <cmath>
#include "../core/model_loading/obj_loader.hpp"
#include "../core/util/util.hpp"
#include "../nodes/voxel.hpp"
#include "../core/renderer/voxelizer.hpp"

BasicScene::~BasicScene() = default;

void BasicScene::ready() {

}

void BasicScene::update(float dt) {

}

// void BasicScene::onReady() {
//     // * Object loading example
//     // Object to voxelize
//     ObjLoader loader;
//     MeshData meshData = loader.Load(
//         "C:\\Users\\lipov\\Downloads\\Studanka2\\Studanka2.obj",
//         "C:\\Users\\lipov\\Downloads\\Studanka2\\Studanka2.mtl"
//     );
//
//     Voxelizer voxelizer;
//     auto voxels = voxelizer.voxelize(meshData, 0.1f);
//
//     for (auto voxelGPU : voxels) {
//         Voxel voxel {
//             voxelGPU.position,
//             voxelGPU.size,
//             voxelGPU.color
//         };
//
//         this->meshManager.add(voxel.buildMeshData());
//     }
//
//     // * Directional sun example
//     Light sun;
//     sun.type = LightType::Directional;
//     sun.direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.5f));
//     sun.color = glm::vec3(1.0f, 1.0f, 1.0f);
//     sun.energy = 1.0f;
//
//     // Register light to the scene
//     this->lighting.lights.addLight(sun);
//
//     // * Point light example
//     Light point;
//     point.type = LightType::Point;
//     point.position = glm::vec3(0.0f, 2.0f, 0.0f);
//     point.color = glm::vec3(1.0f, 0.0f, 0.0f);
//     point.energy = 50.0f;
//     point.range = 3.0f;
//
//     // Register light to the scene
//     this->lighting.lights.addLight(point);
// }
