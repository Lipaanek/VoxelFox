#pragma once

#include <string>

using ResourceID = unsigned int;

struct ResourceLoadInfo {
    virtual ~ResourceLoadInfo() = default;
};
struct AssetRefs {};

class Resource {
public:
    virtual ~Resource() = default;

    virtual bool loadFromFile(const ResourceLoadInfo& loadInfo) = 0;

    [[nodiscard]] ResourceID getID() const { return id; }
    [[nodiscard]] const std::string& getPath() const { return path; }

protected:
    ResourceID id = 0;
    std::string path;

    AssetRefs assetRefs;

private:
    friend class ResourceManager;
};