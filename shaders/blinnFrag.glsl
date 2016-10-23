#version 430 core

in vec2 UV;
in vec3 normal;

layout( location = 0 ) out vec4 fragColour;

void main()
{
    vec4 baseColour = vec4(1.0);
    //Get dot.
    float mul = dot(normal, vec3(0.0, 1.0, 0.0));
    //Fit to range 0-1
    mul += 1.0;
    mul /= 2.0;
    mul *= 1.5;
    baseColour.xyz *= mul;

    fragColour = baseColour;
}
