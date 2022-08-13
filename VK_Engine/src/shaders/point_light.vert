#version 450

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, -1.0),
  vec2(1.0, -1.0),
  vec2(-1.0, 1.0),
  vec2(1.0, 1.0)
);

layout (location = 0) out vec2 fragOffset;

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
    vec4 position;
    vec4 color;
    float radius;
} push;

void main() {
    fragOffset = OFFSETS[gl_VertexIndex];

    vec3 cameraRightWorld = {ubo.ViewMatrix[0][0],ubo.ViewMatrix[1][0],ubo.ViewMatrix[2][0]};
    vec3 cameraUpWorld = {ubo.ViewMatrix[0][1],ubo.ViewMatrix[1][1],ubo.ViewMatrix[2][1]};

    vec3 positionWorld = push.position.xyz 
        + push.radius * fragOffset.x * cameraRightWorld
        + push.radius * fragOffset.y * cameraUpWorld;

        gl_Position = ubo.projectionMatrix * ubo.ViewMatrix * vec4(positionWorld, 1.0);
}