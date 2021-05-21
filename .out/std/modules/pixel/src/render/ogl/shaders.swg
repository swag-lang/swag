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

    uniform mat4    mvp;

    out vec4        vcolor;
    out vec2        vpixelPos;
    out vec4        vaa0;
    out vec4        vaa1;
    out vec4        vaa2;
    out vec4        vaa3;
    out vec4        vaa4;
    out vec4        vaa5;

    void main()
    {
        gl_Position = mvp * vec4(vertexPosition, 0, 1);
        vpixelPos = vertexPosition.xy;
        vcolor = vertexColor;
        vaa0 = aa0;
        vaa1 = aa1;
        vaa2 = aa2;
        vaa3 = aa3;
        vaa4 = aa4;
        vaa5 = aa5;
    }
"@

const PixelShaderAA = @"
    #version 330 core
    in vec4     vcolor;
    in vec2     vpixelPos;
    in vec4     vaa0;
    in vec4     vaa1;
    in vec4     vaa2;
    in vec4     vaa3;
    in vec4     vaa4;
    in vec4     vaa5;

    out vec4    color;

    float computeAlphaAA(vec4 segment)
    {
        float norm;
        vec2 es = segment.zw - segment.xy;
        if (es.x != 0 || es.y != 0)
        {
            norm = sqrt(es.x * es.x + es.y * es.y);
            vec2  es1 = es / norm;
            vec2  ptmp  = vpixelPos - segment.xy;
            float dot   = (es1.x * ptmp.x) + (es1.y * ptmp.y);
            float norm2 = dot < 0 ? 1 : 0;
            dot /= norm;
            norm2 = dot > 1 ? 1 : norm2;
            float norm1 = abs((es.x * (segment.y - vpixelPos.y)) - (es.y * (segment.x - vpixelPos.x))) / norm;
            return clamp(norm1 + norm2, 0, 1);
        }

        return 1;
    }

    float computeAlphaEdgesAA()
    {
        float norm;
        norm = min(1,    computeAlphaAA(vaa0));
        norm = min(norm, computeAlphaAA(vaa1));
        norm = min(norm, computeAlphaAA(vaa2));
        norm = min(norm, computeAlphaAA(vaa3));
        norm = min(norm, computeAlphaAA(vaa4));
        norm = min(norm, computeAlphaAA(vaa5));
        return norm;
    }

    void main()
    {
        color = vcolor;
        color.w *= computeAlphaEdgesAA();
    }
"@

impl RenderOgl
{
    func createProgram(using self, vertex, pixel: string)->GLint
    {
        var result: GLint

        vertexShaderID := glCreateShader(GL_VERTEX_SHADER)
        vertexSourcePointer := @dataof(vertex)
        glShaderSource(vertexShaderID, 1, &vertexSourcePointer, null)
        glCompileShader(vertexShaderID)

        fragmentShaderID := glCreateShader(GL_FRAGMENT_SHADER)
        pixelSourcePointer := @dataof(pixel)
        glShaderSource(fragmentShaderID, 1, &pixelSourcePointer, null)
        glCompileShader(fragmentShaderID)

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
        programID = createProgram(VertexShaderAA, PixelShaderAA)
    }
}