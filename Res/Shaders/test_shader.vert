#version 460

layout (location = 0) in vec4 a_Location;
layout (location = 1) in vec3 a_Color;

layout (location = 0) out vec3 o_Color;

layout (binding = 0) uniform MatrixBuffer {
    mat4 model;
    mat4 view;
    mat4 proj;
} matBuffer;


void main() {
    gl_Position = matBuffer.proj * matBuffer.view * a_Location;
    o_Color = a_Color;
}
