#include "catch_amalgamated.hpp"

#include "core/resources/resource_manager.hpp"

namespace {

struct TestResourceLoadInfo : ResourceLoadInfo {
    bool shouldLoad = true;
};

class TestResource : public Resource {
public:
    bool loadFromFile(const ResourceLoadInfo& loadInfo) override {
        const auto* info = dynamic_cast<const TestResourceLoadInfo*>(&loadInfo);
        return info && info->shouldLoad;
    }
};

class OtherTestResource : public TestResource {};

}

TEST_CASE("ResourceManager loads, retrieves, and unloads typed resources") {
    ResourceManager manager;
    const TestResourceLoadInfo loadInfo;

    const auto resource = manager.load<TestResource>(loadInfo);
    REQUIRE(resource);
    REQUIRE(resource->getID() != 0);
    CHECK(manager.getResource<TestResource>(resource->getID()) == resource);
    CHECK_FALSE(manager.getResource<OtherTestResource>(resource->getID()));

    manager.unload(resource->getID());
    CHECK_FALSE(manager.getResource<TestResource>(resource->getID()));
}

TEST_CASE("ResourceManager does not cache resources that fail to load") {
    ResourceManager manager;
    TestResourceLoadInfo loadInfo;
    loadInfo.shouldLoad = false;

    CHECK_FALSE(manager.load<TestResource>(loadInfo));
}
