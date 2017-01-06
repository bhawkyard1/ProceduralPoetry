#version 410 core

in vec4 position;
in vec2 UV;
in float generic;

uniform vec4 camPos;

layout( location = 0 ) out vec4 outDiffuse;
layout( location = 1 ) out vec4 outDepth;

void main()
{
    outDepth = vec4(distance(camPos, position));
    outDiffuse = vec4(1.0,1.0,1.0,UV.x * generic * 0.04);
}
