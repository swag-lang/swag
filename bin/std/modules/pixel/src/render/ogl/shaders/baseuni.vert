layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 uv1;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec4 aa0;
layout(location = 4) in vec4 aa1;
layout(location = 5) in vec4 aa2;
layout(location = 6) in vec4 aa3;
layout(location = 7) in vec4 aa4;
layout(location = 8) in vec4 aa5;
layout(location = 9) in float aanum;

uniform mat4    mvp;
uniform mat4    mdl;
uniform vec4    boundRect;
uniform vec4    textureRect;
uniform float   uvMode;
