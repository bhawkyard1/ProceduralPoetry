#version 410 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

out vec2 UV;
out vec3 normal;
out vec4 position;

uniform mat4 M;
uniform mat4 MVP;
uniform mat3 normalMat;

void main()
{
    gl_Position = MVP * inPosition;
    UV = inUV;
    normal = /*normalMat */ inNormal;
    position = M * inPosition;
}
