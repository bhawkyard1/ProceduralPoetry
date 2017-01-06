#version 410 core

in vec4 position;
in vec2 UV;
in float generic;

uniform vec4 camPos;

layout( location = 0 ) out vec4 outDiffuse;
layout( location = 1 ) out vec4 outDepth;

void main()
{
    outDiffuse = vec4(1.0,1.0,1.0,UV.x * generic * 0.02);
    outDepth = vec4((camPos - position).length());
}
