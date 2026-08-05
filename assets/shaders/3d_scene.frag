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
uniform float u_lightEnergy[MAX_LIGHTS];
uniform float u_lightRanges[MAX_LIGHTS];

uniform vec3 u_cameraPos;

uniform float u_shininess;
uniform vec3 u_skyColor;
uniform vec3 u_groundColor;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(u_cameraPos - vWorldPos);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < u_lightCount; i++) {
        vec3 lightDir;
        float lightPower;

        if (u_lightTypes[i] == 0) {
            lightDir = normalize(-u_lightDirections[i]); // directional light
            lightPower = u_lightEnergy[i];
        } else {
            vec3 toLight = u_lightPositions[i] - vWorldPos;
            float dist = length(toLight);
            lightDir = toLight / dist;

            // Inverse-square falloff, clamped near the light
            lightPower = u_lightEnergy[i] / max(dist * dist, 0.01);

            // Smooth range cutoff: 1 at d=0, 0 at d=range
            float falloff = max(1.0 - pow(dist / u_lightRanges[i], 4.0), 0.0);
            falloff *= falloff;
            lightPower *= falloff;
        }

        float nDotL = max(dot(normal, lightDir), 0.0);
        diffuse += u_lightColors[i] * lightPower * nDotL;

        // Blinn-Phong specular highlight
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfDir), 0.0), u_shininess);
        specular += u_lightColors[i] * lightPower * spec;
    }

    vec3 ambient = mix(u_groundColor, u_skyColor, normal.y * 0.5f + 0.5f);

    vec3 color = vColor * (ambient + diffuse + specular);

    // Reinhard tone mapping + gamma correction so highlights don't blow out
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
