#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in float inGeneric;

out vec4 position;
out vec2 UV;
out float generic;

uniform mat4 M;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(inPosition.xyz, 1.0);
    position = M * vec4(inPosition, 1.0);
    UV = inUV;
    generic = inGeneric;
}
