#include "catch_amalgamated.hpp"

#include "core/lighting/lighting.hpp"
#include "core/renderer/frame_data_collector.hpp"

TEST_CASE("Frame lighting data reflects scene lighting settings") {
    Lighting lighting;
    lighting.setShininess(12.0f);
    lighting.setSkyColor({ 0.6f, 0.7f, 0.8f });
    lighting.setGroundColor({ 0.1f, 0.2f, 0.3f });

    const LightingData data = FrameDataCollector::collectLightingData(lighting);
    REQUIRE(data.shininess == Catch::Approx(12.0f));
    REQUIRE(data.skyColor == glm::vec3(0.6f, 0.7f, 0.8f));
    REQUIRE(data.groundColor == glm::vec3(0.1f, 0.2f, 0.3f));
}
