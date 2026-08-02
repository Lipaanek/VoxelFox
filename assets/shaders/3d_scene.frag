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

uniform vec3 u_cameraPos;

// Cheap bounced-light fake: upward faces pick up cool sky light,
// downward faces fall to a warm-dark ground tone.
const vec3 SKY_COLOR = vec3(0.30f, 0.35f, 0.45f);
const vec3 GROUND_COLOR = vec3(0.08f, 0.08f, 0.10f);

const float SHININESS = 32.0f;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(u_cameraPos - vWorldPos);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

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

        float nDotL = max(dot(normal, lightDir), 0.0);
        diffuse += u_lightColors[i] * u_lightIntensities[i] * nDotL * attenuation;

        // Blinn-Phong specular highlight
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfDir), 0.0), SHININESS);
        specular += u_lightColors[i] * u_lightIntensities[i] * spec * attenuation;
    }

    vec3 ambient = mix(GROUND_COLOR, SKY_COLOR, normal.y * 0.5f + 0.5f);

    vec3 color = vColor * (ambient + diffuse + specular);

    // Reinhard tone mapping + gamma correction so highlights don't blow out
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
