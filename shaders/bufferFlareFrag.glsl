#version 410 core

#define MAX_LIGHTS 512

in vec2 UV;

uniform sampler2D diffuse;
uniform sampler2D noiseTex;
uniform mat4 VP;

uniform int activeLights;

uniform vec2 resolution;

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

layout( location = 0 ) out vec4 fragColour;

// musk's lense flare, modified by icecool.
// "Modularized" by SolarLiner, with improvements
// See the original at: https://www.shadertoy.com/view/4sX3Rs

#define ORB_FLARE_COUNT	6
#define DISTORTION_BARREL		1.0

vec2 GetDistOffset(vec2 uv, vec2 pxoffset)
{
    vec2 tocenter = uv.xy;
    vec3 prep = normalize(vec3(tocenter.y, -tocenter.x, 0.0));

    float angle = length( tocenter.xy ) * 2.221 * DISTORTION_BARREL;
    vec3 oldoffset = vec3(pxoffset,0.0);

    vec3 rotated = oldoffset * cos(angle) + cross(prep, oldoffset) * sin(angle) + prep * dot(prep, oldoffset) * (1.0-cos(angle));

    return rotated.xy;
}

float glare(vec2 uv, vec2 pos, float size)
{
    vec2 main = uv - pos;

    float dist = length(main);
    float powdist = pow(dist,0.1);

    float f0 = 1.0 /(dist * ( 1.0 / size * 16.0 )+ 1.0 );

    return f0 + f0 * (1.1 + dist * 0.1 );
}

vec3 flare(vec2 uv, vec2 pos, float dist, float size)
{
    pos = GetDistOffset(uv, pos);

    float t = 1.0 / (size * 2.0);

    float r = max(0.01-pow(length(uv +(dist - 0.05) * pos),2.4) * t,0.0)*6.0;
    float g = max(0.01-pow(length(uv + dist         * pos),2.4) * t,0.0)*6.0;
    float b = max(0.01-pow(length(uv +(dist + 0.05) * pos),2.4) * t,0.0)*6.0;

    return vec3(r, g, b);
}
vec3 flare(vec2 uv, vec2 pos, float dist, float size, vec3 color)
{
    return flare(uv, pos, dist, size)*color*4.0;
}

vec3 orb(vec2 uv, vec2 pos, float dist, float size)
{
    vec3 c = vec3(0.0);

    for(int i=0; i<ORB_FLARE_COUNT; i++)
    {
        float j = float(i+1);
        float offset = j/(j+1.);
        float colOffset = j/float(ORB_FLARE_COUNT*2);

        c += flare(uv,pos,dist+offset, size/(j+.1), vec3(1.0-colOffset, 1.0, 0.5+colOffset));
    }

    c += flare(uv,pos,dist+.5, 4.0*size, vec3(1.0))*4.0;

    return c / 4.0;
}
vec3 orb(vec2 uv, vec2 pos, float dist, float size, vec3 color)
{
    return orb(uv,pos,dist,size)*color;
}

vec3 ring(vec2 uv, vec2 pos, float dist)
{
    vec2 uvd = uv*(length(uv));

    float r = max(1.0/(1.0+32.0*pow(length(uvd+(dist-.05)*pos),2.0)),.0)*00.25;
    float g = max(1.0/(1.0+32.0*pow(length(uvd+ dist     *pos),2.0)),.0)*00.23;
    float b = max(1.0/(1.0+32.0*pow(length(uvd+(dist+.05)*pos),2.0)),.0)*00.21;

    return vec3(r,g,b);
}

vec3 lensflare(vec2 uv, vec2 pos, float brightness, float size)
{
    vec3 c = vec3(glare(uv, pos, size));

    c += flare(uv,pos,-3.0,3.0*size) * 4.0;
    c += flare(uv,pos, -1.0,size) * 12.0;
    c += flare(uv,pos, 0.5,0.8*size) * 4.0;
    c += flare(uv,pos,-0.4,0.8*size) * 4.0;

    c += orb(uv,pos, 0.0, 0.5*size);

    c += ring(uv,pos,-1.0)*0.5*size;
    c += ring(uv,pos, 1.0)*0.5*size;

    return c*brightness;
}

vec3 cc(vec3 color, float factor,float factor2) // color modifier
{
    float w = color.x+color.y+color.z;
    return mix(color,vec3(w)*factor,w*factor2);
}

void main()
{
    fragColour = texture(diffuse, UV);

    vec3 colour = vec3(0.0);

    vec2 aspectUV = UV - 0.5;
    aspectUV.x *= resolution.x / resolution.y;

    for(int i = 0; i < activeLights; i++)
    {
        vec4 lp = (VP * vec4(lbuf.buf[i].pos.xyz, 1.0));
        lp = lp / lp.w;
        lp.xy /= 2.0;
        //lp.xy += vec2(0.5);
        lp.x *= resolution.x / resolution.y;

        colour += lensflare( aspectUV, lp.xy, lbuf.buf[i].lum * 0.0025, 1.0);
    }
    fragColour.xyz += pow(colour, vec3(1.2));
    //fragColour = texture(diffuse, UV);
    fragColour.a = 1.0;
}
