#version 430 core

in vec2 UV;
in vec3 normal;

uniform vec4 baseColour;

layout( location = 0 ) out vec4 fragColour;

void main()
{
    //Get dot.
    float mul = dot(normal, vec3(0.0, 1.0, 0.0));
    //Fit to range 0-1
    mul /= 4.0;
    mul += 0.75;
    vec4 diffuse = baseColour;
    diffuse.xyz *= mul;

    fragColour = diffuse;
}
