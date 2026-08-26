#include <memory>

#include "core/window/window.hpp"
#include "core/scene/scene_manager.hpp"
#include "core/scene/editor.hpp"
#include "scene/basic_scene.hpp"
#include "core/renderer/shader_program.hpp"
#include "core/util/util.hpp"
#include "core/input/input_system.hpp"
#include "core/model_loading/obj_loader.hpp"
#include "core/scripting/lua_engine.hpp"
#include "nodes/mesh_instance_3d.hpp"
#include "nodes/node3d.hpp"

std::unique_ptr<Environment> currentEnvironment;

int main() {
    Window window("VoxelFox", 1920, 1080);

    MeshManager meshManager;
    MeshRenderer meshRenderer(meshManager);

    // Setup editor
    SceneManager sceneManager(window, meshRenderer);
    InputSystem input(window.getHandle());
    currentEnvironment = std::make_unique<Editor>(
        sceneManager,
        window,
        input
    );

    // Shader program and shader creation
    ShaderProgram program;
    Shader frag {"assets/shaders/3d_scene.frag", ShaderType::Fragment};
    Shader vert{"assets/shaders/3d_scene.vert", ShaderType::Vertex};

    // Need to compile the shaders before linking
    frag.compile();
    vert.compile();

    if (frag.getID() == 0 || vert.getID() == 0) {
        Util::Log::error("Failed to compile shaders");
        return 1;
    }
        
    // Attach and link programs
    program.attach(vert);
    program.attach(frag);
    program.link();

    // Init scene
    std::unique_ptr<Scene> scene = std::make_unique<BasicScene>();

    // Make nodes
    auto root = std::make_unique<Node3D>();
    auto mesh = std::make_unique<MeshInstance3D>();
    mesh->setName("studanka");

    // Load mesh
    ObjLoader loader;
    MeshData meshData = loader.Load(
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.obj)",
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.mtl)"
    );

    // Register mesh (better for sharing the same meshes)
    MeshID id = meshManager.add(meshData);

    // Setup mesh and add node to tree
    mesh->setMesh(id);
    root->addChild(std::move(mesh));

    // Set the tree root
    scene->setRoot(std::move(root));

    // Test script attachment
    if (auto* wellMesh = scene->getRoot()->getChild("studanka")) {
        auto res = wellMesh->setScript(
            "assets/scripts/test_node_script.lua",
            {}
        );
    }

    currentEnvironment->setScene(std::move(scene));

    double lastTime = glfwGetTime();
    while (!window.shouldClose()) {
        input.beginInput();
        window.update();

        double now = glfwGetTime();
        auto dt = static_cast<float>(now - lastTime);
        lastTime = now;

        currentEnvironment->update(dt);

        // Clear screen from previous frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render
        RenderContext ctx {
            .program = program,
            .camera = currentEnvironment->getCamera(),
            .window = window
        };
        currentEnvironment->render(ctx);

        // Display frame
        window.present();
    }

    return 0;
}
