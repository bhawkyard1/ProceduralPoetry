#version 430 core

in vec2 UV;
in vec3 normal;

uniform vec4 baseColour;

//layout( location = 0 ) out vec4 fragColour;

layout( location = 0 ) out vec4 outDiffuse;
layout( location = 1 ) out vec4 outNormal;
layout( location = 2 ) out vec4 outPosition;

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
    outPosition = gl_FragCoord;
}
