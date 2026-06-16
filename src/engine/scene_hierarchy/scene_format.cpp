#include "scene_format.hpp"
#include "scene.hpp"
#include "nodes/node.hpp"
#include "nodes/node3d.hpp"
#include "nodes/mesh_instance_3d.hpp"
#include "nodes/light_3d.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>

static std::string Trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static int GetIndent(const std::string& line) {
    int count = 0;
    for (char c : line) {
        if (c == ' ') count++;
        else if (c == '\t') count += 4;
        else break;
    }
    return count;
}

static std::unique_ptr<Node> CreateNodeByType(const std::string& type) {
    if (type == "Node3D") return std::make_unique<Node3D>();
    if (type == "MeshInstance3D") return std::make_unique<MeshInstance3D>();
    if (type == "Light3D") return std::make_unique<Light3D>();
    return std::make_unique<Node>();
}

static bool WritePropertyValue(std::ofstream& out, const std::string& key, const std::string& value, int indent) {
    std::string pad(indent, ' ');
    out << pad << "property \"" << key << "\" = " << value << "\n";
    return true;
}

static std::string Vec3ToString(const glm::vec3& v) {
    std::ostringstream oss;
    oss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return oss.str();
}

static std::string FloatToString(float f) {
    std::ostringstream oss;
    oss << f;
    return oss.str();
}

static std::string StringToString(const std::string& s) {
    return "\"" + s + "\"";
}

static void SerializeNode(std::ofstream& out, const Node* node, int indent) {
    std::string pad(indent, ' ');
    std::string type = node->GetNodeType();
    out << pad << "node \"" << type << "\" \"" << node->name << "\"\n";
    out << pad << "{\n";

    int propIndent = indent + 4;
    int childIndent = indent + 4;

    if (auto* n3d = dynamic_cast<const Node3D*>(node)) {
        WritePropertyValue(out, "position", Vec3ToString(n3d->position), propIndent);
        WritePropertyValue(out, "eulerRotation", Vec3ToString(n3d->eulerRotation), propIndent);
        WritePropertyValue(out, "scale", Vec3ToString(n3d->scale), propIndent);

        if (auto* mi = dynamic_cast<const MeshInstance3D*>(node)) {
            WritePropertyValue(out, "mesh", StringToString(mi->meshAssetPath), propIndent);
        }

        if (auto* l = dynamic_cast<const Light3D*>(node)) {
            WritePropertyValue(out, "color", Vec3ToString(l->color), propIndent);
            WritePropertyValue(out, "intensity", FloatToString(l->intensity), propIndent);
        }
    }

    for (const auto& child : node->children) {
        SerializeNode(out, child.get(), childIndent);
    }

    out << pad << "}\n";
}

static bool ParseProperty(const std::string& line, std::string& key, std::string& value) {
    size_t propPos = line.find("property");
    if (propPos == std::string::npos) return false;

    size_t keyStart = line.find('"', propPos);
    if (keyStart == std::string::npos) return false;
    size_t keyEnd = line.find('"', keyStart + 1);
    if (keyEnd == std::string::npos) return false;
    key = line.substr(keyStart + 1, keyEnd - keyStart - 1);

    size_t eqPos = line.find('=', keyEnd);
    if (eqPos == std::string::npos) return false;

    value = Trim(line.substr(eqPos + 1));
    return true;
}

static glm::vec3 ParseVec3(const std::string& s) {
    glm::vec3 result(0.0f);
    std::string trimmed = s;
    if (trimmed.front() == '(') trimmed = trimmed.substr(1);
    if (trimmed.back() == ')') trimmed.pop_back();

    std::istringstream iss(trimmed);
    char comma;
    iss >> result.x >> comma >> result.y >> comma >> result.z;
    return result;
}

static float ParseFloat(const std::string& s) {
    return std::stof(s);
}

static std::string ParseString(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

static void ApplyProperty(Node* node, const std::string& key, const std::string& value) {
    if (auto* n3d = dynamic_cast<Node3D*>(node)) {
        if (key == "position") n3d->position = ParseVec3(value);
        else if (key == "eulerRotation") n3d->eulerRotation = ParseVec3(value);
        else if (key == "scale") n3d->scale = ParseVec3(value);

        if (auto* mi = dynamic_cast<MeshInstance3D*>(node)) {
            if (key == "mesh") mi->meshAssetPath = ParseString(value);
        }

        if (auto* l = dynamic_cast<Light3D*>(node)) {
            if (key == "color") l->color = ParseVec3(value);
            else if (key == "intensity") l->intensity = ParseFloat(value);
        }
    }
}

bool SceneFormat::Save(const Scene& scene, const std::string& filePath) {
    std::ofstream out(filePath);
    if (!out.is_open()) {
        printf("Failed to write scene file: %s\n", filePath.c_str());
        return false;
    }

    out << "scene \"Untitled Scene\"\n{\n";
    for (const auto& child : scene.GetRoot()->children) {
        SerializeNode(out, child.get(), 4);
    }
    out << "}\n";

    out.close();
    return true;
}

std::unique_ptr<Scene> SceneFormat::Load(const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        printf("Failed to read scene file: %s\n", filePath.c_str());
        return nullptr;
    }

    auto scene = std::make_unique<Scene>();
    std::vector<Node*> nodeStack;
    std::vector<int> indentStack;
    Node* currentNode = scene->GetRoot();
    int currentIndent = -1;

    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = Trim(line);
        if (trimmed.empty()) continue;

        if (trimmed == "{") {
            indentStack.push_back(currentIndent);
            continue;
        }

        if (trimmed == "}") {
            if (!indentStack.empty()) {
                indentStack.pop_back();
            }
            if (!nodeStack.empty()) {
                currentNode = nodeStack.back();
                nodeStack.pop_back();
            } else {
                currentNode = scene->GetRoot();
            }
            continue;
        }

        if (trimmed.find("scene") == 0) {
            continue;
        }

        int indent = GetIndent(line);

        if (trimmed.find("node ") == 0) {
            size_t firstQuote = trimmed.find('"');
            size_t secondQuote = trimmed.find('"', firstQuote + 1);
            size_t thirdQuote = trimmed.find('"', secondQuote + 1);
            size_t fourthQuote = trimmed.find('"', thirdQuote + 1);

            if (firstQuote != std::string::npos && secondQuote != std::string::npos &&
                thirdQuote != std::string::npos && fourthQuote != std::string::npos) {
                std::string type = trimmed.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                std::string name = trimmed.substr(thirdQuote + 1, fourthQuote - thirdQuote - 1);

                auto newNode = CreateNodeByType(type);
                newNode->name = name;

                Node* newNodePtr = newNode.get();
                if (currentNode) {
                    currentNode->AddChild(std::move(newNode));
                }

                nodeStack.push_back(currentNode);
                currentNode = newNodePtr;
                currentIndent = indent;
            }
            continue;
        }

        std::string key, value;
        if (ParseProperty(trimmed, key, value)) {
            if (currentNode) {
                ApplyProperty(currentNode, key, value);
            }
        }
    }

    in.close();
    return scene;
}
