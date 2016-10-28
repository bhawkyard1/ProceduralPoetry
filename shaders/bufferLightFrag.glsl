#version 430 core

in vec2 UV;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;

layout( location = 0 ) out vec4 fragColour;

void main()
{
    //Get dot.
    vec3 normVec = texture(normal, UV).xyz;
    float mul = dot(normVec, vec3(0.0, 1.0, 0.0));
    //Fit to range 0-1
    mul /= 4.0;
    mul += 0.75;
    vec4 diffVec = texture(diffuse, UV);
    diffVec.xyz *= mul;

    fragColour = diffVec;
}
