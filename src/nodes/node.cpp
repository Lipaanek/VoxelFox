#include "node.hpp"

#include <cstring>

#include "../core/scene/scene.hpp"
#include "../core/util/util.hpp"

LoadScriptResult Node::setScript(const char* path, const std::vector<std::string>& flags) {
    if (!scene) {
        Util::Log::error("Cannot set script: Node has no Scene");
        return {};
    }

    script_ = std::make_unique<LuaScript>(scene->lua());

    auto result = script_->setScript(path, flags);

    if (result.result == LoadResult::Success) {
        scene->lua().addScript(script_.get());
    } else {
        Util::Log::error(result.message);
        script_.reset();
    }

    return result;
}

Node* Node::getChild(const std::string& childName) const {
    for (const auto& child : children) {
        if (child->getName() == childName) {
            return child.get();
        }
    }

    return nullptr;
}

