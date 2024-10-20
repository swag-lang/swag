#version 330 core
#include base

in vec4     vcolor;
in vec2     vuv0;
in vec2     vuv1;
in vec4     vaa0;
in vec4     vaa1;
in vec4     vaa2;
in vec4     vaa3;
in vec4     vaa4;
in vec4     vaa5;
in vec4     vaa6;
in vec4     vaa7;
in vec4     vaa8;
in vec4     vaa9;
in vec4     vaa10;
in vec4     vaa11;
in float    vaanum;
in float    bcopymode;

layout(origin_upper_left) in vec4 gl_FragCoord;

uniform sampler2D   inTexture0;
uniform sampler2D   inTexture1;
uniform bool        copyMode;

out vec4 color;

// Compute the distance from the pixel to the segment
float computeAlphaAA(vec4 segment)
{
    vec2  seg  = segment.zw - segment.xy;
    float normSeg = sqrt(seg.x * seg.x + seg.y * seg.y);
    vec2  segNormalized = seg / normSeg;
    vec2  startToPixel = gl_FragCoord.xy - segment.xy;
    float dot = (segNormalized.x * startToPixel.x) + (segNormalized.y * startToPixel.y);

    // If outside segment, take distance to the point
    if(dot < 0)
    {
        float normDiff = sqrt(startToPixel.x * startToPixel.x + startToPixel.y * startToPixel.y);
        return clamp(normDiff, 0, 1);
    }

    if(dot > normSeg)
    {
        startToPixel = gl_FragCoord.xy - segment.zw;
        float normDiff = sqrt(startToPixel.x * startToPixel.x + startToPixel.y * startToPixel.y);
        return clamp(normDiff, 0, 1);
    }

    vec2  projSeg = segNormalized * dot;
    vec2  diffSeg = startToPixel - projSeg;
    float normDiff = sqrt(diffSeg.x * diffSeg.x + diffSeg.y * diffSeg.y);
    return clamp(normDiff, 0, 1);
}

float computeAlphaEdgesAA()
{
    float norm = 1;
    if(vaanum < 0.5) return norm;
    norm = min(norm, computeAlphaAA(vaa0));
    if(vaanum < 1.5) return norm;
    norm = min(norm, computeAlphaAA(vaa1));
    if(vaanum < 2.5) return norm;
    norm = min(norm, computeAlphaAA(vaa2));
    if(vaanum < 3.5) return norm;
    norm = min(norm, computeAlphaAA(vaa3));
    if(vaanum < 4.5) return norm;
    norm = min(norm, computeAlphaAA(vaa4));
    if(vaanum < 5.5) return norm;
    return min(norm, computeAlphaAA(vaa5));
    if(vaanum < 6.5) return norm;
    return min(norm, computeAlphaAA(vaa6));
    if(vaanum < 7.5) return norm;
    return min(norm, computeAlphaAA(vaa7));
    if(vaanum < 8.5) return norm;
    return min(norm, computeAlphaAA(vaa8));
    if(vaanum < 9.5) return norm;
    return min(norm, computeAlphaAA(vaa9));
    if(vaanum < 10.5) return norm;
    return min(norm, computeAlphaAA(vaa10));
    if(vaanum < 11.5) return norm;
    return min(norm, computeAlphaAA(vaa11));
}

void main()
{
    color = vcolor * texture(inTexture0, vuv0);
    color.w *= computeAlphaEdgesAA();
    color.w *= texture(inTexture1, vuv1).r;
    if(color.w == 0 && !copyMode)
        discard;
}