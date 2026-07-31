#version 330 core
#define MAX_LIGHTS 8

in vec3 vColor;
in vec3 vNormal;
in vec3 vWorldPos;

out vec4 FragColor;

uniform int u_lightCount;
uniform int u_lightTypes[MAX_LIGHTS];
uniform vec3 u_lightPositions[MAX_LIGHTS];
uniform vec3 u_lightDirections[MAX_LIGHTS];
uniform vec3 u_lightColors[MAX_LIGHTS];
uniform float u_lightIntensities[MAX_LIGHTS];
uniform vec3 u_lightAttenuations[MAX_LIGHTS];

const float AMBIENT = 0.5f;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 diffuse = vec3(0.0);

    for (int i = 0; i < u_lightCount; i++) {
        vec3 lightDir;
        float attenuation = 1.0;

        if (u_lightTypes[i] == 0) {
            lightDir = normalize(-u_lightDirections[i]); // directional light
        } else {
            vec3 toLight = u_lightPositions[i] - vWorldPos;
            float dist = length(toLight);
            lightDir = normalize(toLight);

            attenuation = 1.0 / (
                    u_lightAttenuations[i].x
                +   u_lightAttenuations[i].y * dist
                +   u_lightAttenuations[i].z * dist * dist
            );
        }

        diffuse += u_lightColors[i] * u_lightIntensities[i]
            * max(dot(normal, lightDir), 0.0) * attenuation;
    }
    vec3 color = vColor * (AMBIENT + clamp(diffuse, 0.0, 1.0));
    FragColor = vec4(color, 1.0);
}