#include "catch_amalgamated.hpp"

#include <memory>
#include <string>
#include <vector>

#include "core\scripting\events\lua_event_bindings.hpp"
#include "core\scripting\lua_api_registry.hpp"
#include "core\scripting\lua_engine.hpp"
#include "core\scripting\vector\lua_vector3.hpp"
#include "core\scene\scene.hpp"
#include "nodes\node3d.hpp"

namespace {

std::string eventString(const Util::Event& event, const char* key) {
    return std::get<std::string>(event.data.at(key));
}

double eventNumber(const Util::Event& event, const char* key) {
    const auto& value = event.data.at(key);
    if (const auto* number = std::get_if<double>(&value))
        return *number;
    if (const auto* integer = std::get_if<std::int64_t>(&value))
        return static_cast<double>(*integer);
    FAIL("Event field is not numeric");
    return 0.0;
}

}

TEST_CASE("Node scripts isolate lifecycle globals and emit instance-bound events") {
    Scene scene;
    auto root = std::make_unique<Node3D>("Root");
    auto firstNode = std::make_unique<Node3D>("First");
    auto secondNode = std::make_unique<Node3D>("Second");
    Node3D* first = firstNode.get();
    Node3D* second = secondNode.get();
    root->addChild(std::move(firstNode));
    root->addChild(std::move(secondNode));
    scene.setRoot(std::move(root));

    LuaApiRegistry registry;
    registry.addModule(&LuaVector3Bindings::registerApi);
    registry.addModule(&LuaEventBindings::registerApi);
    scene.lua().registerApiBindings(registry, { .eventBus = &scene.eventBus() });

    std::vector<std::string> readyEvents;
    std::vector<std::string> pongEvents;
    std::vector<double> tickValues;
    scene.eventBus().subscribe<Util::Event>(
        "test.ready",
        [&readyEvents](const Util::Event& event) {
            readyEvents.push_back(eventString(event, "who"));
        }
    );
    scene.eventBus().subscribe<Util::Event>(
        "test.pong",
        [&pongEvents](const Util::Event& event) {
            pongEvents.push_back(eventString(event, "who"));
            REQUIRE(eventNumber(event, "value") == 17.0);
        }
    );
    scene.eventBus().subscribe<Util::Event>(
        "test.tick",
        [&tickValues](const Util::Event& event) {
            tickValues.push_back(eventNumber(event, "ticks"));
        }
    );

    REQUIRE(first->setScript("tests/assets/node_instance_a.lua", {}).result == LoadResult::Success);
    REQUIRE(second->setScript("tests/assets/node_instance_b.lua", {}).result == LoadResult::Success);

    scene.lua().runReady();
    REQUIRE(first->getPosition() == glm::vec3(1.0f, 2.0f, 3.0f));
    REQUIRE(second->getPosition() == glm::vec3(4.0f, 5.0f, 6.0f));
    const std::vector<std::string> expectedReady{ "a", "b" };
    REQUIRE(readyEvents == expectedReady);

    Util::Event ping;
    ping.data.emplace("value", std::int64_t{17});
    scene.eventBus().emit("test.ping", ping);
    scene.eventBus().emit("test.ping", ping);
    const std::vector<std::string> expectedPong{ "a", "b" };
    REQUIRE(pongEvents == expectedPong);

    scene.lua().runUpdate(0.25f);
    REQUIRE(tickValues.size() == 2);
    REQUIRE(tickValues[0] == Catch::Approx(0.25));
    REQUIRE(tickValues[1] == Catch::Approx(10.25));
}

TEST_CASE("Removing a node unregisters its per-node Lua lifecycle") {
    Scene scene;
    auto root = std::make_unique<Node3D>("Root");
    auto child = std::make_unique<Node3D>("Child");
    Node3D* childPointer = child.get();
    root->addChild(std::move(child));
    scene.setRoot(std::move(root));

    LuaApiRegistry registry;
    registry.addModule(&LuaVector3Bindings::registerApi);
    registry.addModule(&LuaEventBindings::registerApi);
    scene.lua().registerApiBindings(registry, { .eventBus = &scene.eventBus() });

    REQUIRE(childPointer->setScript("tests/assets/node_instance_a.lua", {}).result == LoadResult::Success);
    int pongCount = 0;
    int tickCount = 0;
    scene.eventBus().subscribe<Util::Event>(
        "test.tick",
        [&tickCount](const Util::Event&) { ++tickCount; }
    );
    scene.eventBus().subscribe<Util::Event>(
        "test.pong",
        [&pongCount](const Util::Event&) { ++pongCount; }
    );
    scene.lua().runReady();

    scene.getRoot()->removeChild(childPointer);
    Util::Event ping;
    ping.data.emplace("value", std::int64_t{17});
    scene.eventBus().emit("test.ping", ping);
    scene.lua().runUpdate(0.1f);
    REQUIRE(pongCount == 0);
    REQUIRE(tickCount == 0);
}
