#version 330 core
in vec4     vcolor;
in vec2     vuv0;
in vec2     vuv1;
in vec4     vaa0;
in vec4     vaa1;
in vec4     vaa2;
in vec4     vaa3;
in vec4     vaa4;
in vec4     vaa5;
in float    vaanum;

layout(origin_upper_left) in vec4 gl_FragCoord;

uniform sampler2D inTexture0;
uniform sampler2D inTexture1;
uniform vec3      pixelSize;

out vec4    color;

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
}

vec3 energy_distribution(vec4 previous, vec4 current, vec4 next)
{
    float primary   = 32/100.0;
    float secondary = 25/100.0;
    float tertiary  = 9/100.0;

    // Energy distribution as explained on:
    // http://www.grc.com/freeandclear.htm
    //
    //  .. v..
    // RGB RGB RGB
    // previous.g + previous.b + current.r + current.g + current.b
    //
    //   . .v. .
    // RGB RGB RGB
    // previous.b + current.r + current.g + current.b + next.r
    //
    //     ..v ..
    // RGB RGB RGB
    // current.r + current.g + current.b + next.r + next.g

    float r =
        tertiary  * previous.g +
        secondary * previous.b +
        primary   * current.r  +
        secondary * current.g  +
        tertiary  * current.b;

    float g =
        tertiary  * previous.b +
        secondary * current.r +
        primary   * current.g  +
        secondary * current.b  +
        tertiary  * next.r;

    float b =
        tertiary  * current.r +
        secondary * current.g +
        primary   * current.b +
        secondary * next.r    +
        tertiary  * next.g;

    return vec3(r,g,b);
}

void main()
{
    vec4 inColor = vcolor * texture(inTexture0, vuv0);
    inColor.w *= computeAlphaEdgesAA();

    if(pixelSize.z == 0)
    {
        inColor.w *= texture(inTexture1, vuv1).r;
        color = inColor;
        return;
    }

    vec4 current = texture(inTexture1, vuv1);
    float r = current.r;
    float g = current.g;
    float b = current.b;

    vec4 tcolor;
    tcolor.r = mix(r, inColor.r, current.r);
    tcolor.g = mix(g, inColor.g, current.g);
    tcolor.b = mix(b, inColor.b, current.b);
    tcolor.a = mix((r+g+b)/3.0, min(min(r,g),b), max(max(r,g),b));

    color = vec4(tcolor.rgb, inColor.a * tcolor.a);
}