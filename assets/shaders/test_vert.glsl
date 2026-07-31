#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 vColor;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);
    vColor = aColor;
}
