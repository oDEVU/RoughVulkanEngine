#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 0) out vec4 out_color;

layout(push_constant) uniform Push {
    mat4 transform;
    mat4 normalMatrix;
} push;

void main() {
    out_color = vec4(fragColor, 1.0);
}