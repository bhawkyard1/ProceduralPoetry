#version 410 core

in vec4 position;
in vec2 UV;
in float generic;

uniform vec4 camPos;

layout( location = 0 ) out vec4 outDiffuse;
layout( location = 1 ) out float outDepth;

void main()
{
    outDepth = distance(camPos.xyz, position.xyz);
    outDiffuse = vec4(1.0, 1.0, 1.0, UV.x * generic * 0.04);
}