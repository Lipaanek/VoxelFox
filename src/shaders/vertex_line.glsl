#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 VertexColor;
out vec3 FragPos;

void main() {
    VertexColor = aColor;
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    gl_Position = projection * view * worldPos;
}
