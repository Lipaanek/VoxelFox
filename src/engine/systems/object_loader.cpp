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

// Helper function
vector<string> split(const string& str) {
    vector<string> tokens;
    istringstream iss(str);
    string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void parseVertexLine(const string& line) {
    vector<string> tokens = split(line);

    float x = stof(tokens[1]);
    float y = stof(tokens[2]);
    float z = stof(tokens[3]);

    vertices.emplace_back(
        glm::vec3(x, y, z),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f)
    );
}

void parseFillLine(string line) {
    vector<string> tokens = split(line);

    // Vertex indexes
    int vi1 = stoi(tokens[1]);
    int vi2 = stoi(tokens[2]);
    int vi3 = stoi(tokens[3]);

    // Vertexes
    Vertex v1 = vertices.at(vi1 - 1);
    Vertex v2 = vertices.at(vi2 - 1);
    Vertex v3 = vertices.at(vi3 - 1);

    worldData.emplace_back(v1);
    worldData.emplace_back(v2);
    worldData.emplace_back(v3);
}

void processLine(string line) {
    if (line[0] == 'v') {
        parseVertexLine(line);
    } else if (line[0] == 'f') {
        parseFillLine(line);
    }
}

Mesh loadObject(string path) {
    ifstream file(path);

    string line;

    if (file.is_open()) {
        // Read each line from the file, store in line str
        while (getline(file, line)) {
            processLine(line);
        }

        file.close();
    }
    else {
        printf("Failed to read file");
    }

    Mesh mesh;

    return mesh;
}

vector<Vertex> getWorldData() {
    return worldData;
}