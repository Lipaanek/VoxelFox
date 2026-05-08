#include <fstream>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <sstream>

#include "../../headers/object_loader.hpp"
#include "../../headers/vertex.hpp"

using namespace std;

vector<Vertex> vertices;
vector<Vertex> worldData;
vector<glm::vec3> normals;

// Split line by whitespace
vector<string> split(const string& str) {
    vector<string> tokens;
    istringstream iss(str);
    string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Clear all the data once finished loading
void clearScene() {
    vertices.clear();
    normals.clear();
    worldData.clear();
}

void parseVertexLine(const string& line) {
    vector<string> tokens = split(line);

    if (tokens.size() < 4) return;

    float x = stof(tokens[1]);
    float y = stof(tokens[2]);
    float z = stof(tokens[3]);

    vertices.emplace_back(
        glm::vec3(x, y, z),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(0.0f)
    );
}

void parseNormalLine(const string& line) {
    vector<string> tokens = split(line);
    if (tokens.size() < 4) return;
    
    float x = stof(tokens[1]);
    float y = stof(tokens[2]);
    float z = stof(tokens[3]);
    
    normals.emplace_back(x, y, z);
}

void parseFillLine(const string& line) {
    vector<string> tokens = split(line);
    int numIndices = tokens.size() - 1;

    if (numIndices < 3) return;

    // Parse all vertex indices from the face
    vector<pair<int, int>> faceIndices;
    for (int i = 1; i <= numIndices; i++) {
        size_t firstSlash = tokens[i].find('/');
        if (firstSlash == string::npos) {
            faceIndices.emplace_back(stoi(tokens[i]) - 1, -1);
        } else {
            int vertexIdx = stoi(tokens[i].substr(0, firstSlash)) - 1;
            size_t secondSlash = tokens[i].find('/', firstSlash + 1);
            int normalIdx = -1;
            if (secondSlash != string::npos) {
                normalIdx = stoi(tokens[i].substr(secondSlash + 1)) - 1;
            }
            faceIndices.emplace_back(vertexIdx, normalIdx);
        }
    }

    // Triangulate N-gon: create triangles (0,1,2), (0,2,3), (0,3,4), ...
    for (int i = 1; i < numIndices - 1; i++) {
        int vi1 = faceIndices[0].first;
        int vi2 = faceIndices[i].first;
        int vi3 = faceIndices[i + 1].first;

        if (vi1 < 0 || vi2 < 0 || vi3 < 0) continue;
        if (vi1 >= static_cast<int>(vertices.size()) ||
            vi2 >= static_cast<int>(vertices.size()) ||
            vi3 >= static_cast<int>(vertices.size())) continue;

        // Determine normal for this triangle
        glm::vec3 normal(0.0f, 1.0f, 0.0f);
        int ni1 = faceIndices[0].second;
        if (ni1 >= 0 && ni1 < static_cast<int>(normals.size())) {
            normal = normals[ni1];
        }

        // Create the three vertices for this triangle
        Vertex v1 = vertices[vi1];
        v1.normal = normal;
        worldData.push_back(v1);

        Vertex v2 = vertices[vi2];
        v2.normal = normal;
        worldData.push_back(v2);

        Vertex v3 = vertices[vi3];
        v3.normal = normal;
        worldData.push_back(v3);
    }
}

void processLine(string line) {
    if (line[0] == 'v') {
        if (line[1] == 'n') {
            parseNormalLine(line);
        } else {
            parseVertexLine(line);
        }
    } else if (line[0] == 'f') {
        parseFillLine(line);
    }
}

void loadObject(string path) {
    clearScene();
    
    ifstream file(path);

    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            processLine(line);
        }

        file.close();
    }
    else {
        printf("Failed to read file");
    }
}

vector<Vertex> getWorldData() {
    return worldData;
}