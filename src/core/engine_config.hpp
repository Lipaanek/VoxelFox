#pragma once

#include <string_view>

// Don't forget to change those in new releases
namespace EngineConfig {
    inline constexpr std::string_view ENGINE_NAME = "VoxelFox";
    inline constexpr unsigned int PROUD_VERSION = 1;
    inline constexpr unsigned int DEFAULT_VERSION = 9;
    inline constexpr unsigned int SHAME_VERSION = 1;
    inline const std::string ENGINE_VERSION =
        std::to_string(PROUD_VERSION) + "." +
        std::to_string(DEFAULT_VERSION) + "." +
        std::to_string(SHAME_VERSION)
    ;
}