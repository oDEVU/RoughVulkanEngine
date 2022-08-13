#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 out_color;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionMatrix;
    mat4 ViewMatrix;
    mat4 InverseViewMatrix;
    vec4 ambienLight;
    PointLight pointLights[256];
    int numLights;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    vec3 diffueseLight = ubo.ambienLight.xyz * ubo.ambienLight.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPos = ubo.InverseViewMatrix[3].xyz;
    vec3 viewDirection = normalize(cameraPos - fragPosWorld);

    for (int i = 0; i < ubo.numLights; i++){
        PointLight light = ubo.pointLights[i];
        vec3 directionToLight = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);

        directionToLight = normalize(directionToLight);

        float cosAngInc = max(dot(surfaceNormal, directionToLight), 0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffueseLight += intensity * cosAngInc;

        //specular
        vec3 falfAngle = normalize(directionToLight + viewDirection);
        float blinTerm = dot(surfaceNormal, falfAngle);
        blinTerm = clamp(blinTerm, 0, 1);
        blinTerm = pow(blinTerm, 512.0);

        specularLight += intensity * blinTerm;
    }

    out_color = vec4(diffueseLight * fragColor + specularLight * fragColor, 1.0);
}