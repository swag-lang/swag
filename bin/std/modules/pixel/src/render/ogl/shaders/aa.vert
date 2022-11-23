#version 330 core
#include base

out vec4  vcolor;
out vec2  vuv0;
out vec2  vuv1;
out vec4  vaa0;
out vec4  vaa1;
out vec4  vaa2;
out vec4  vaa3;
out vec4  vaa4;
out vec4  vaa5;
out vec4  vaa6;
out vec4  vaa7;
out vec4  vaa8;
out vec4  vaa9;
out vec4  vaa10;
out vec4  vaa11;
out float vaanum;

vec4 computeTransformAA(vec4 aa)
{
    vec4 v0 = mdl * vec4(aa.xy, 0, 1);
    vec4 v1 = mdl * vec4(aa.zw, 0, 1);
    return vec4(v0.xy, v1.xy);
}

void transformAA()
{
    vaanum = aanum;
    if(vaanum < 0.5) return;
    vaa0 = computeTransformAA(aa0);
    if(vaanum < 1.5) return;
    vaa1 = computeTransformAA(aa1);
    if(vaanum < 2.5) return;
    vaa2 = computeTransformAA(aa2);
    if(vaanum < 3.5) return;
    vaa3 = computeTransformAA(aa3);
    if(vaanum < 4.5) return;
    vaa4 = computeTransformAA(aa4);
    if(vaanum < 5.5) return;
    vaa5 = computeTransformAA(aa5);
    if(vaanum < 6.5) return;
    vaa6 = computeTransformAA(aa6);
    if(vaanum < 7.5) return;
    vaa7 = computeTransformAA(aa7);
    if(vaanum < 8.5) return;
    vaa8 = computeTransformAA(aa8);
    if(vaanum < 9.5) return;
    vaa9 = computeTransformAA(aa9);
    if(vaanum < 10.5) return;
    vaa10 = computeTransformAA(aa10);
    if(vaanum < 11.5) return;
    vaa11 = computeTransformAA(aa11);
}

void main()
{
    gl_Position = mvp * mdl * vec4(vertexPosition, 0, 1);

    // source is rgba, and we need bgra
    vcolor = vertexColor.zyxw;

    // uv
    vuv0 = computeUVs(vertexPosition.xy);
    vuv1.x = uv1.x;
    vuv1.y = 1 - uv1.y;

    transformAA();
}