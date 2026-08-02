#include "basic_screen.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "../core/model_loading/obj_loader.hpp"
#include "../nodes/voxel.hpp"

void BasicScreen::render() {
    float time = static_cast<float>(glfwGetTime());

    // Per-object transform: animate a translation on the second voxel.
    // Baking alone can't move an object without rebuilding its vertices.
    /* meshManager.setTransform(this->voxel2UID,
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, std::sin(time), 0.0f))); */

    // View/projection and lighting setup
    this->program.setUniform("u_view", this->camera.getViewMatrix());
    this->program.setUniform("u_projection", this->camera.getProjectionMatrix(this->window.getAspect()));
    this->program.setUniform("u_cameraPos", this->camera.getPosition());

    this->lights.uploadLights(this->program);

    // Drawing
    meshManager.render(this->program);
}

void BasicScreen::onReady() {
    // * Object loading example
    //ObjLoader loader;
    //MeshData meshData = loader.Load(
    //    "C:\\Users\\lipov\\Downloads\\nl9ixa6iwyrk-WoodenLarry\\WoodenLarry.obj",
    //    "C:\\Users\\lipov\\Downloads\\nl9ixa6iwyrk-WoodenLarry\\WoodenLarry.mtl"
    //);

    // * Single voxel example
    Voxel voxel1 { 
        { 2.0f, 0.0f, 0.0f },   // XYZ
          0.5f,                 // Size
        { 1.0f, 1.0f, 1.0f }    // Color
    };

    meshManager.add(voxel1.buildMeshData());

    Voxel voxel2 { { 0.0f, -7.0f, -5.0f }, 10.0f, { 1.0f, 1.0f, 1.0f } };

    this->voxel2UID = meshManager.add(voxel2.buildMeshData());

    // * Directional sun example
    Light sun;
    sun.type = LightType::Directional;
    sun.direction = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.5f));
    sun.color = glm::vec3(1.0f, 1.0f, 1.0f);
    sun.intensity = 1.0f;

    // Register light to the scene
    this->sunIndex = this->lights.addLight(sun);

    // * Point light example
    Light point;
    point.type = LightType::Point;
    point.position = glm::vec3(0.0f, 2.0f, 0.0f);
    point.color = glm::vec3(1.0f, 0.0f, 0.0f);
    point.intensity = 10.0f;
    point.attenuation = glm::vec3(1.0f, 0.0f, 0.1f);

    // Register light to the scene
    this->pointIndex = this->lights.addLight(point);
}
