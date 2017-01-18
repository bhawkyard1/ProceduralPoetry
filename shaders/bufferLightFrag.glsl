#version 410 core

#define MAX_LIGHTS 512

in vec2 UV;

uniform vec3 camPos;
uniform vec3 camDir;

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D position;
uniform sampler2D radius;
uniform sampler2D emissive;

uniform int activeLights;

uniform vec3 rimLightCol;

vec3 sssCol = vec3(0.8, 0.1, 0.2);
vec3 fogCol = vec3(0.01, 0.01, 0.0125);
float fogDist = 1024.0;

layout( location = 0 ) out vec4 outDiffuse;
layout( location = 1 ) out float outDepth;

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

float dotLight( vec3 lv, vec3 nv, float tolerance )
{
    //Normal based multiplier.
    float mul = dot( lv, nv ) + tolerance;
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
    float mul = dotLight( diff, snorm, 0.0f );
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
    litCol += sssCol * sssMul;

    return litCol;
}

dirLight mainLight = dirLight( normalize(vec3(0.3, 0.2, 0.0)), vec3(0.15, 0.19, 0.2), 0.4 );
dirLight accentLight = dirLight( normalize(vec3(-0.5, -0.2, 0.3)), vec3(0.1, 0.4, 0.3), 0.2 );
dirLight fillLight = dirLight( camDir, rimLightCol, 2.0 );

void main()
{
    if(texture(radius, UV).r == 0.0)
    {
        outDiffuse = vec4(fogCol.xyz, 1.0);
        outDepth = 1.0 / 0.0;
        return;
    }

    vec3 spos = texture(position, UV).xyz;
    float depth = distance(spos, camPos);
    outDepth = depth;
    /*if(depth > 250 && depth < 500)
    {
        outDiffuse = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }*/

    //Get dot.
    vec3 normVec = texture(normal, UV).xyz;
    vec3 lightcol = vec3(0.0);

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

    lightcol += dotLight( mainLight.dir, normVec, 0.1 ) * mainLight.col * mainLight.lum;
    lightcol += dotLight( accentLight.dir, normVec, 0.1 ) * accentLight.col * accentLight.lum;
    lightcol += dotLight( fillLight.dir, normVec, 0.1 ) * fillLight.col * fillLight.lum;

    float t = clamp(depth / fogDist, 0.0, 1.0);

    //Luminance of the pixel (values greater than one, emissive light).
    vec3 luminance = texture(emissive, UV).xyz;

    //The base colour, ie values clamped to below one.
    //vec3 base = texture(diffuse, UV).xyz * lightcol;
    vec3 base = (1.0 - t) * (texture(diffuse, UV).xyz * lightcol) + t * fogCol;

    outDiffuse.xyz = base + luminance;

    //outDiffuse.xyz = luminance;
    outDiffuse.a = 1.0;
}
