#include "obj_loader.hpp"
#include "../util/util.hpp"

#include <glm/glm.hpp>
#include <functional>
#include <map>
#include <optional>
#include <unordered_map>

namespace {

    using MaterialColors = std::map<std::string, glm::vec3>;

    // Unique (position, texCoord, normal) reference used to deduplicate
    // corners shared between faces.
    struct VertexKey {
        int pos;
        int uv;
        int norm;

        bool operator==(const VertexKey&) const = default;
    };

    struct VertexKeyHash {
        size_t operator()(const VertexKey& key) const noexcept {
            size_t h = std::hash<int>{}(key.pos);
            h ^= std::hash<int>{}(key.uv) + 0x9e3779b9u + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(key.norm) + 0x9e3779b9u + (h << 6) + (h >> 2);
            return h;
        }
    };

    std::string directoryOf(const std::string& path) {
        size_t slash = path.find_last_of("/\\");
        if (slash == std::string::npos)
            return "";
        return path.substr(0, slash + 1);
    }

    bool parseFloat(const std::string& token, float& out) {
        try {
            out = std::stof(token);
            return true;
        } catch (...) {
            return false;
        }
    }

    // OBJ indices are 1-based; negative indices count back from the end.
    int resolveIndex(int index, size_t count) {
        if (index > 0)
            return index - 1;
        return static_cast<int>(count) + index;
    }

    std::optional<int> resolveIndexChecked(int index, size_t count) {
        if (index == 0)
            return std::nullopt;
        int resolved = resolveIndex(index, count);
        if (resolved < 0 || static_cast<size_t>(resolved) >= count)
            return std::nullopt;
        return resolved;
    }

    // Parse a face index token: "v", "v/vt", "v//vn" or "v/vt/vn".
    // A missing component leaves that index at 0 (meaning "not referenced").
    bool parseIndexToken(const std::string& token, int& pos, int& uv, int& norm) {
        pos = uv = norm = 0;

        size_t first = token.find('/');
        size_t second = (first == std::string::npos) ? std::string::npos : token.find('/', first + 1);

        try {
            pos = std::stoi(token.substr(0, first));

            if (first != std::string::npos) {
                size_t uvEnd = (second == std::string::npos) ? token.size() : second;
                std::string uvStr = token.substr(first + 1, uvEnd - first - 1);
                if (!uvStr.empty())
                    uv = std::stoi(uvStr);
            }
            if (second != std::string::npos) {
                std::string normStr = token.substr(second + 1);
                if (!normStr.empty())
                    norm = std::stoi(normStr);
            }
        } catch (...) {
            return false;
        }
        return true;
    }

    void parseMtllib(const std::string& objDir, const std::string& mtlPath, MaterialColors& materials) {
        std::string contents = Util::File::read(objDir + mtlPath);
        if (contents.empty()) {
            Util::Log::error("Failed to read material file: " + objDir + mtlPath);
            return;
        }

        std::string currentName;
        for (const std::string& rawLine : Util::String::split(contents, "\n")) {
            std::vector<std::string> tokens = Util::String::splitWhitespace(rawLine);
            if (tokens.empty())
                continue;

            if (tokens[0] == "newmtl" && tokens.size() >= 2) {
                currentName = tokens[1];
            } else if (tokens[0] == "Kd" && !currentName.empty() && tokens.size() >= 4) {
                float r, g, b;
                if (parseFloat(tokens[1], r) && parseFloat(tokens[2], g) && parseFloat(tokens[3], b))
                    materials[currentName] = glm::vec3(r, g, b);
            }
        }
    }
}

MeshData ObjLoader::Load(const std::string& filePath, const std::string& mtlPath) {
    MeshData data;

    std::string contents = Util::File::read(filePath);
    if (contents.empty()) {
        Util::Log::error("Failed to read OBJ file: " + filePath);
        return data;
    }

    std::string objDir = directoryOf(filePath);
    MaterialColors materials;

    // An explicit MTL path overrides the "mtllib" references in the OBJ.
    if (!mtlPath.empty())
        parseMtllib("", mtlPath, materials);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

    glm::vec3 currentColor(1.0f);

    // Maps a unique (position, texCoord, normal) tuple to a vertex index
    // so that corners shared between faces are not duplicated.
    std::unordered_map<VertexKey, GLuint, VertexKeyHash> vertexLookup;

    for (const std::string& rawLine : Util::String::split(contents, "\n")) {
        std::vector<std::string> tokens = Util::String::splitWhitespace(rawLine);
        if (tokens.empty())
            continue;

        const std::string& prefix = tokens[0];

        if (prefix == "mtllib" && tokens.size() >= 2 && mtlPath.empty()) {
            parseMtllib(objDir, tokens[1], materials);
        } else if (prefix == "usemtl" && tokens.size() >= 2) {
            auto it = materials.find(tokens[1]);
            currentColor = (it != materials.end()) ? it->second : glm::vec3(1.0f);
        } else if (prefix == "v" && tokens.size() >= 4) {
            float x, y, z;
            if (parseFloat(tokens[1], x) && parseFloat(tokens[2], y) && parseFloat(tokens[3], z))
                positions.emplace_back(x, y, z);
        } else if (prefix == "vt" && tokens.size() >= 3) {
            float u, v;
            if (parseFloat(tokens[1], u) && parseFloat(tokens[2], v))
                texCoords.emplace_back(u, v);
        } else if (prefix == "vn" && tokens.size() >= 4) {
            float x, y, z;
            if (parseFloat(tokens[1], x) && parseFloat(tokens[2], y) && parseFloat(tokens[3], z))
                normals.emplace_back(x, y, z);
        } else if (prefix == "f") {
            std::vector<int> facePos, faceUv, faceNorm;
            bool valid = true;
            for (size_t i = 1; i < tokens.size(); ++i) {
                int p, t, n;
                if (!parseIndexToken(tokens[i], p, t, n)) {
                    valid = false;
                    break;
                }
                facePos.push_back(p);
                faceUv.push_back(t);
                faceNorm.push_back(n);
            }
            if (!valid || facePos.size() < 3)
                continue;

            std::vector<glm::vec3> facePositions;
            for (int p : facePos) {
                auto idx = resolveIndexChecked(p, positions.size());
                if (!idx) {
                    facePositions.clear();
                    break;
                }
                facePositions.push_back(positions[*idx]);
            }
            if (facePositions.size() < 3)
                continue;

            // Fan-triangulate: supports triangles, quads and n-gons.
            for (size_t i = 1; i + 1 < facePos.size(); ++i) {
                int cornerPos[3] = { facePos[0], facePos[i], facePos[i + 1] };
                int cornerUv[3] = { faceUv[0], faceUv[i], faceUv[i + 1] };
                int cornerNorm[3] = { faceNorm[0], faceNorm[i], faceNorm[i + 1] };

                // Fallback normal when the OBJ provides no per-vertex normals.
                glm::vec3 flatNormal = glm::normalize(glm::cross(
                    facePositions[i] - facePositions[0],
                    facePositions[i + 1] - facePositions[0]));

                for (int c = 0; c < 3; ++c) {
                    int posIdx = resolveIndex(cornerPos[c], positions.size());
                    auto uvIdx = resolveIndexChecked(cornerUv[c], texCoords.size());
                    auto normIdx = resolveIndexChecked(cornerNorm[c], normals.size());

                    auto key = VertexKey{ posIdx, uvIdx.value_or(-1), normIdx.value_or(-1) };
                    auto it = vertexLookup.find(key);
                    if (it != vertexLookup.end()) {
                        data.indices.push_back(it->second);
                        continue;
                    }

                    glm::vec2 uv = uvIdx ? texCoords[*uvIdx] : glm::vec2(0.0f);
                    glm::vec3 normal = normIdx ? normals[*normIdx] : flatNormal;

                    GLuint newIndex = static_cast<GLuint>(data.vertices.size());
                    data.vertices.emplace_back(positions[posIdx], normal, uv, currentColor);
                    vertexLookup[key] = newIndex;
                    data.indices.push_back(newIndex);
                }
            }
        }
    }

    return data;
}
