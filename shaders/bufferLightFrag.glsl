#version 410 core

#define MAX_LIGHTS 512

in vec2 UV;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;

uniform int activeLights;

layout( location = 0 ) out vec4 fragColour;

struct light
{
    vec4 pos;
    vec3 col;
    float lum;
};

layout( std140 ) uniform lightBuffer
{
    light buffer [MAX_LIGHTS];
} buffer;

struct dirLight
{
    vec3 dir;
    vec3 col;
    float lum;
};

float dotLight( vec3 lv, vec3 nv )
{
    //Normal based multiplier.
    float mul = dot( lv, nv );
    mul = clamp( mul, 0.0f, 1.0f );

    return mul;
}

float dotLightN( vec3 lv, vec3 nv )
{
    lv = normalize(lv);
    return dotLight(lv, nv);
}

vec3 computeLighting( vec3 spos, vec3 snorm, vec3 lpos, vec3 lcol, float llum )
{
    vec3 diff = lpos - spos;
    float len = diff.length();
    diff /= len;

    //Normal based multiplier.
    float mul = dotLight( diff, snorm );

    //Quadratic attenuation.
    mul = (llum * mul) / pow(len, 2.0);

    return mul * lcol;
}

dirLight mainLight = dirLight( normalize(vec3(0.1, 0.9, 0.0)), vec3(0.1, 0.1, 0.2), 1.0 );
dirLight accentLight = dirLight( normalize(vec3(-0.5, -0.2, 0.3)), vec3(1.0, 0.0, 0.4), 0.2);

void main()
{
    //Get dot.
    vec3 normVec = texture(normal, UV).xyz;
    vec3 lightcol = vec3(0.0);

    for(int i = 0; i < activeLights; i++)
    {
        lightcol += computeLighting(
                    texture(position, UV).xyz,
                    normVec,
                    buffer.buffer[i].pos.xyz,
                    buffer.buffer[i].col,
                    buffer.buffer[i].lum
                    );
    }

    lightcol += dotLight( mainLight.dir, normVec ) * mainLight.col * mainLight.lum;
    lightcol += dotLight( accentLight.dir, normVec ) * accentLight.col * accentLight.lum;

    vec4 diffVec = texture(diffuse, UV);
    diffVec.xyz += lightcol;

    fragColour = diffVec;
}
