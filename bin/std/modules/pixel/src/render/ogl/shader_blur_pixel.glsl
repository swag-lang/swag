#version 120

uniform sampler2D inTexture0;

//in  vec2 vuv0;
//out vec4 color;

void main()
{
    gl_FragColor = texture2D(inTexture0, gl_TexCoord[0].st);
    gl_FragColor.xyz *= 0.7;
}