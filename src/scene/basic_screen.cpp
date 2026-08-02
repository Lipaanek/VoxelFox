#include "basic_screen.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../core/model_loading/obj_loader.hpp"
#include "../nodes/voxel.hpp"

void BasicScreen::render() {
    float time = static_cast<float>(glfwGetTime());

    // Model rotation
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    // Model 3D uniform setup
    this->program.setUniform("u_model", model);
    this->program.setUniform("u_view", this->camera.getViewMatrix());
    this->program.setUniform("u_projection", this->camera.getProjectionMatrix(this->window.getAspect()));
    this->lights.uploadLights(this->program);

    // Drawing
    mesh.draw(this->program);
}

void BasicScreen::onReady() {
    // * Object loading example
    //ObjLoader loader;
    //MeshData meshData = loader.Load(
    //    "C:\\Users\\lipov\\Downloads\\nl9ixa6iwyrk-WoodenLarry\\WoodenLarry.obj",
    //    "C:\\Users\\lipov\\Downloads\\nl9ixa6iwyrk-WoodenLarry\\WoodenLarry.mtl"
    //);

    // * Single voxel example
    Voxel voxel{ 
        { 0.0f, 0.0f, 0.0f },   // XYZ
          0.5f,                 // Size
        { 1.0f, 1.0f, 1.0f }    // Color
    };
    
    MeshData meshData = voxel.buildMeshData();

    // * Point light example
    Light point;
    point.type = LightType::Point;
    point.position = glm::vec3(2.0f, 1.0f, 2.0f);
    point.color = glm::vec3(1.0f, 1.0f, 1.0f);
    point.intensity = 3.0f;

    // Register light to the scene
    this->lights.addLight(point);

    // Set the mesh scene
    mesh.setVertices(meshData.vertices);
    mesh.setIndices(meshData.indices);
    mesh.setup();
}
