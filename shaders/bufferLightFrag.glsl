#version 410 core

#define MAX_LIGHTS 512

in vec2 UV;

uniform vec3 camPos;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D radius;

uniform int activeLights;

layout( location = 0 ) out vec4 outDiffuse;
layout( location = 1 ) out vec4 outDepth;

struct light
{
    vec4 pos;
    vec3 col;
    float lum;
};

layout( std140 ) uniform lightBuffer
{
    light buf [MAX_LIGHTS];
} lbuf;

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

vec3 computeLighting( vec3 spos, vec3 snorm, vec3 lpos, vec3 lcol, float llum )
{
    //Get vector between light surface and its length.
    vec3 diff = lpos - spos;
    float len = distance(lpos, spos);
    diff = normalize(diff);

    len = 0.125 * pow(len, 2.0);
    //Normal based multiplier.
    float mul = dotLight( diff, snorm );
    //Quadratic attenuation.
    mul = (llum * mul) / len;
    //Diffuse lit colour
    vec3 litCol = mul * lcol;

    vec3 eye = normalize(camPos - spos);

    //Distance light travels through the object
    float depth = clamp( dot( -diff, snorm ), 0.0, 1.0) * texture(radius, UV).r;

    //How aligned the camera is with the light direction.
    float camVar = clamp( dot( -diff, eye ), 0.0, 1.0 );

    float camPosMul = clamp( dot( snorm, eye ), 0.0, 1.0 );

    float sssMul = llum * camVar * (camPosMul - depth) / len;
    litCol += vec3(0.8, 0.2, 0.2) * sssMul;

    return litCol;
}

dirLight mainLight = dirLight( normalize(vec3(0.1, 0.9, 0.0)), vec3(0.1, 0.1, 0.2), 1.0 );
dirLight accentLight = dirLight( normalize(vec3(-0.5, -0.2, 0.3)), vec3(0.1, 1.0, 0.4), 0.05 );

void main()
{
    if(texture(radius, UV).r == 0.0)
    {
        outDiffuse = vec4( 0.0 );
        outDepth = vec4( 1.0 / 0.0 );
        return;
    }

    vec3 spos = texture(position, UV).xyz;
    outDepth = vec4(distance(spos, camPos));

    //Get dot.
    vec3 normVec = texture(normal, UV).xyz;
    vec3 lightcol = texture(diffuse, UV).xyz;

    if(lightcol.r > 1 && lightcol.g > 1 && lightcol.b > 1)
    {
        outDiffuse = vec4(1.0);
        return;
    }

    for(int i = 0; i < activeLights; i++)
    {
        lightcol += computeLighting(
                    spos,
                    normVec,
                    lbuf.buf[i].pos.xyz,
                    lbuf.buf[i].col,
                    lbuf.buf[i].lum
                    );
    }

    lightcol += dotLight( mainLight.dir, normVec ) * mainLight.col * mainLight.lum;
    lightcol += dotLight( accentLight.dir, normVec ) * accentLight.col * accentLight.lum;

    vec4 diffVec = texture(diffuse, UV);
    diffVec.xyz += lightcol;

    outDiffuse = diffVec;
    /*fragColour = vec4(texture(radius, UV).r);
    fragColour.a = 1.0;*/
}
