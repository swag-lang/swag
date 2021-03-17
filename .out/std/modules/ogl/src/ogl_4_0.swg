#global public

#[extension]
{
    func glBlendEquationSeparatei(buf: GLuint, modeRGB: GLenum, modeAlpha: GLenum);
    func glBlendEquationi(buf: GLuint, mode: GLenum);
    func glBlendFuncSeparatei(buf: GLuint, srcRGB: GLenum, dstRGB: GLenum, srcAlpha: GLenum, dstAlpha: GLenum);
    func glBlendFunci(buf: GLuint, src: GLenum, dst: GLenum);
    func glMinSampleShading(value: GLclampf);
}

const GL_SAMPLE_SHADING = 0x8C36
const GL_MIN_SAMPLE_SHADING_VALUE = 0x8C37
const GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET = 0x8E5E
const GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET = 0x8E5F
const GL_MAX_PROGRAM_TEXTURE_GATHER_COMPONENTS = 0x8F9F
const GL_TEXTURE_CUBE_MAP_ARRAY = 0x9009
const GL_TEXTURE_BINDING_CUBE_MAP_ARRAY = 0x900A
const GL_PROXY_TEXTURE_CUBE_MAP_ARRAY = 0x900B
const GL_SAMPLER_CUBE_MAP_ARRAY = 0x900C
const GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW = 0x900D
const GL_INT_SAMPLER_CUBE_MAP_ARRAY = 0x900E
const GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY = 0x900F