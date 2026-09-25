#include "catch_amalgamated.hpp"

#include <lua.hpp>

#include "core/camera/camera.hpp"
#include "core/renderer/frame_data_collector.hpp"
#include "core/scripting/camera/lua_camera.hpp"
#include "core/scripting/lua_api_registry.hpp"
#include "core/scripting/lua_engine.hpp"
#include "core/scripting/vector/lua_vector3.hpp"

TEST_CASE("Lua camera APIs share the rendered camera across Lua engines") {
    Camera camera;
    LuaApiContext context { .camera = &camera };

    LuaApiRegistry registry;
    registry.addModule(&LuaCameraBindings::registerApi);
    registry.addModule(&LuaVector3Bindings::registerApi);

    LuaEngine editorEngine;
    LuaEngine sceneEngine;
    editorEngine.registerApiBindings(registry, context);
    sceneEngine.registerApiBindings(registry, context);

    REQUIRE(luaL_dostring(editorEngine.state(), "Camera.set_position(Vector3.new(1, 2, 3))") == LUA_OK);
    REQUIRE(luaL_dostring(sceneEngine.state(), "Camera.set_yaw(25)") == LUA_OK);
    REQUIRE(luaL_dostring(sceneEngine.state(), "Camera.set_pitch(-10)") == LUA_OK);

    REQUIRE(camera.getPosition() == glm::vec3(1.0f, 2.0f, 3.0f));
    REQUIRE(camera.getYaw() == Catch::Approx(25.0f));
    REQUIRE(camera.getPitch() == Catch::Approx(-10.0f));

    const CameraRenderData renderCamera = FrameDataCollector::collectCameraData(camera, 16.0f / 9.0f);
    REQUIRE(renderCamera.position == camera.getPosition());
    REQUIRE(renderCamera.view == camera.getViewMatrix());
}
