#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec2 uPosition;
uniform vec2 uSize;
uniform vec2 uScreenSize;

out vec2 vLocalPos;

void main() {
    vec2 pixelPos = uPosition + (aPos + 0.5) * uSize;

    vec2 clipPos = pixelPos / uScreenSize * 2.0 - 1.0;

    vLocalPos = (aPos) * uSize;

    gl_Position = vec4(clipPos, 0.0, 1.0);
}