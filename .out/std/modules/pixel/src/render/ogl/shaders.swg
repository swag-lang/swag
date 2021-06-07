using Core, Ogl, Core.Math

const VertexShaderAA = #load("vertex.glsl")
const PixelShaderAA  = #load("pixel.glsl")

impl RenderOgl
{
    func createProgram(using self, vertex, pixel: const [..] u8)->GLint
    {
        var result: GLint
	    var infoLogLength: s32
        var infoLog: String

        vertexShaderID := glCreateShader(GL_VERTEX_SHADER)
        vertexSourcePointer := @dataof(vertex)
        vertexSourceLength := cast(GLint) @countof(vertex)
        glShaderSource(vertexShaderID, 1, &vertexSourcePointer, &vertexSourceLength)
        glCompileShader(vertexShaderID)

        // Get err
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result)
        glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
        infoLog.reserve(cast(uint) infoLogLength + 1)
        glGetShaderInfoLog(vertexShaderID, infoLogLength, null, @dataof(infoLog))
        if infoLogLength
        {
            infoLog.length = infoLogLength
            Console.print(infoLog)
        }

        fragmentShaderID := glCreateShader(GL_FRAGMENT_SHADER)
        pixelSourcePointer := @dataof(pixel)
        pixelSourceLength := cast(GLint) @countof(pixel)
        glShaderSource(fragmentShaderID, 1, &pixelSourcePointer, &pixelSourceLength)
        glCompileShader(fragmentShaderID)

        // Get err
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result)
        glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength)
        infoLog.reserve(cast(uint) infoLogLength + 1)
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, null, @dataof(infoLog))
        if infoLogLength
        {
            infoLog.length = infoLogLength
            Console.print(infoLog)
        }

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
        glUseProgram(shaderImage)
        text0 := glGetUniformLocation(shaderImage, @dataof("inTexture0"));
        text1 := glGetUniformLocation(shaderImage, @dataof("inTexture1"));
        glUniform1i(text0, 0);
        glUniform1i(text1,  1);
    }
}