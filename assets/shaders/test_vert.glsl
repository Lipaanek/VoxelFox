#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 vColor;
out vec3 vWorldPos;
out vec3 vNormal;

void main()
{
    vec4 worldPos = u_model * vec4(aPos, 1.0);
    gl_Position = u_projection * u_view * worldPos;
    
    vWorldPos = worldPos.xyz;
    vNormal = normalize(transpose(inverse(mat3(u_model))) * aNormal);
    vColor = aColor;
}
