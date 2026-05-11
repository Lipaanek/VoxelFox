#version 430 core
// Set on which index are inputs
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// outputs
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 VertexColor;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transform normal
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aUV;
    VertexColor = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}