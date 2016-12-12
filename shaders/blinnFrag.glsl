#version 410 core

in vec2 UV;
in vec3 normal;
in vec4 position;

uniform vec4 baseColour;
uniform float radius;

//layout( location = 0 ) out vec4 fragColour;

layout( location = 0 ) out vec4 outDiffuse;
layout( location = 1 ) out vec4 outNormal;
layout( location = 2 ) out vec4 outPosition;
layout( location = 3 ) out vec4 outRadius;
layout( location = 4 ) out vec4 outEmissive;

void main()
{
    /*
    //Get dot.
    float mul = dot(normal, vec3(0.0, 1.0, 0.0));
    //Fit to range 0-1
    mul /= 4.0;
    mul += 0.75;
    vec4 diffuse = baseColour;
    diffuse.xyz *= mul;

    fragColour = diffuse;
    */
    outDiffuse = baseColour;
    outNormal = vec4(normal.xyz, 1.0);
    outPosition = position;

    outRadius = vec4(radius);
}
