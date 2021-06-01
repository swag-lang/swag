using Core, Ogl, Core.Math

const VertexShaderAA = @"
    #version 330 core
    layout(location = 0) in vec2 vertexPosition;
    layout(location = 1) in vec4 vertexColor;
    layout(location = 2) in vec4 aa0;
    layout(location = 3) in vec4 aa1;
    layout(location = 4) in vec4 aa2;
    layout(location = 5) in vec4 aa3;
    layout(location = 6) in vec4 aa4;
    layout(location = 7) in vec4 aa5;
    layout(location = 8) in float aanum;

    uniform mat4    mvp;
    uniform mat4    mdl;
    uniform float   uvmode;
    uniform vec4    boundRect;
    uniform vec4    textureRect;

    out vec4        vcolor;
    out vec2        vpixelPos;
    out vec2        vuv;
    out vec4        vaa0;
    out vec4        vaa1;
    out vec4        vaa2;
    out vec4        vaa3;
    out vec4        vaa4;
    out vec4        vaa5;
    out float       vaanum;

    vec2 computeUVs(vec2 pixelPos)
    {
        vec2 uv;

        // Fill
        if(uvmode < 0.5)
        {
            float w = boundRect.z - boundRect.x;
            float h = boundRect.w - boundRect.y;
            uv.x = (pixelPos.x - boundRect.x) / w;
            uv.y = 1 - (pixelPos.y - boundRect.y) / h;
        }

        // FillSubRect
        else if(uvmode < 1.5)
        {
            float w = boundRect.z - boundRect.x;
            float h = boundRect.w - boundRect.y;
            float xt = (pixelPos.x - boundRect.x) / w;
            float yt = (pixelPos.y - boundRect.y) / h;
            uv.x = textureRect.x + (xt * (textureRect.z - textureRect.x));
            uv.y = 1 - (textureRect.y + (yt * (textureRect.w - textureRect.y)));
        }

        // Tile
        else if(uvmode < 2.5)
        {
            uv.x = (pixelPos.x - boundRect.x) / textureRect.z;
            uv.y = 1 - (pixelPos.y - boundRect.y) / textureRect.w;
        }

        return uv;
    }

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
    }

    void main()
    {
        gl_Position = mvp * mdl * vec4(vertexPosition, 0, 1);

        // transform pixel position with 'mdl'
        vec4 vp = mdl * vec4(vertexPosition.xy, 0, 1);
        vpixelPos.xy = vp.xy;

        // source is rgba, and we need bgra
        vcolor = vertexColor.zyxw;

        // uv
        vuv = computeUVs(vertexPosition.xy);

        transformAA();
    }
"@

const PixelShaderAA = @"
    #version 330 core
    in vec4     vcolor;
    in vec2     vpixelPos;
    in vec2     vuv;
    in vec4     vaa0;
    in vec4     vaa1;
    in vec4     vaa2;
    in vec4     vaa3;
    in vec4     vaa4;
    in vec4     vaa5;
    in float    vaanum;

    uniform sampler2D inTexture;

    out vec4    color;

    // Compute the distance from the pixel to the segment
    float computeAlphaAA(vec4 segment)
    {
        vec2  seg  = segment.zw - segment.xy;
        float normSeg = sqrt(seg.x * seg.x + seg.y * seg.y);
        vec2  segNormalized = seg / normSeg;
        vec2  startToPixel = vpixelPos.xy - segment.xy;
        float dot = (segNormalized.x * startToPixel.x) + (segNormalized.y * startToPixel.y);

        // If outside segment, take distance to the point
        if(dot < 0)
        {
            float normDiff = sqrt(startToPixel.x * startToPixel.x + startToPixel.y * startToPixel.y);
            return clamp(normDiff, 0, 1);
        }

        if(dot > normSeg)
        {
            startToPixel = vpixelPos.xy - segment.zw;
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

    void main()
    {
        color = vcolor * texture(inTexture, vuv);
        color.w *= computeAlphaEdgesAA();
    }
"@

impl RenderOgl
{
    func createProgram(using self, vertex, pixel: string)->GLint
    {
        var result: GLint
	    var infoLogLength: s32
        var infoLog: String

        vertexShaderID := glCreateShader(GL_VERTEX_SHADER)
        vertexSourcePointer := @dataof(vertex)
        glShaderSource(vertexShaderID, 1, &vertexSourcePointer, null)
        glCompileShader(vertexShaderID)

    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result)
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
    infoLog.reserve(cast(uint) infoLogLength + 1)
    glGetShaderInfoLog(vertexShaderID, infoLogLength, null, @dataof(infoLog))

        fragmentShaderID := glCreateShader(GL_FRAGMENT_SHADER)
        pixelSourcePointer := @dataof(pixel)
        glShaderSource(fragmentShaderID, 1, &pixelSourcePointer, null)
        glCompileShader(fragmentShaderID)

    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result)
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
    infoLog.reserve(cast(uint) infoLogLength + 1)
    glGetShaderInfoLog(fragmentShaderID, infoLogLength, null, @dataof(infoLog))

        progID := glCreateProgram()
        glAttachShader(progID, vertexShaderID)
        glAttachShader(progID, fragmentShaderID)
        glLinkProgram(progID)

        glDetachShader(progID, vertexShaderID)
        glDetachShader(progID, fragmentShaderID)
        glDeleteShader(vertexShaderID)
        glDeleteShader(fragmentShaderID)

        return progID
    }

    func createShaders(using self)
    {
        shaderImage = createProgram(VertexShaderAA, PixelShaderAA)
    }
}