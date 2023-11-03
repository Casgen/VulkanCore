#version 460

layout (location = 0) in vec2 a_Location;
layout (location = 1) in vec3 a_Color;

layout (location = 0) out vec3 o_Color;

layout (binding = 0) uniform MatrixBuffer {
    mat4 model;
    mat4 view;
    mat4 proj;
} matBuffer;

vec3 vertices[3] = vec3[](
    vec3(-0.5f, -0.5f, 0.0f),
    vec3(0.f, 0.5f, 0.0f),
    vec3(0.5f, -0.5f, 0.0f)
);

vec3 colors[3] = vec3[](
    vec3(1.0,0.0,0.0),
    vec3(0.0,1.0,0.0),
    vec3(0.0,0.0,1.0)
);


void main() {
    gl_Position = matBuffer.proj * matBuffer.view * vec4(a_Location, 0.5f, 1.0f);
    //gl_Position = vec4(a_Location, 0.f, 1.0f);
    o_Color = a_Color;
}
