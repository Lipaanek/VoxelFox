#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

namespace Util {

using EventValue = std::variant<bool, std::int64_t, double, std::string>;
using EventData = std::unordered_map<std::string, EventValue>;

struct Event {
    virtual ~Event() = default;

    EventData data;
};

} // namespace Util
