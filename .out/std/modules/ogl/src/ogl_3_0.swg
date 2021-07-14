#global public

#[Extension]
{
    func glBeginConditionalRender(id: GLuint, mode: GLenum);
    func glBeginTransformFeedback(primitiveMode: GLenum);
    func glBindFragDataLocation(program: GLuint, colorNumber: GLuint, name: const *GLchar);
    func glClampColor(target: GLenum, clamp: GLenum);
    func glClearBufferfi(buffer: GLenum, drawBuffer: GLint, depth: GLfloat, stencil: GLint);
    func glClearBufferfv(buffer: GLenum, drawBuffer: GLint, value: const *GLfloat);
    func glClearBufferiv(buffer: GLenum, drawBuffer: GLint, value: const *GLint);
    func glClearBufferuiv(buffer: GLenum, drawBuffer: GLint, value: const *GLuint);
    func glColorMaski(buf: GLuint, red: GLboolean, green: GLboolean, blue: GLboolean, alpha: GLboolean);
    func glDisablei(cap: GLenum, index: GLuint);
    func glEnablei(cap: GLenum, index: GLuint);
    func glEndConditionalRender();
    func glEndTransformFeedback();
    func glGetBooleani_v(pname: GLenum, index: GLuint, data: *GLboolean);
    func glGetFragDataLocation(program: GLuint, name: const *GLchar)->GLint;
    func glGetStringi(name: GLenum, index: GLuint)->const *GLubyte;
    func glGetTexParameterIiv(target: GLenum, pname: GLenum, params: *GLint);
    func glGetTexParameterIuiv(target: GLenum, pname: GLenum, params: *GLuint);
    func glGetTransformFeedbackVarying(program: GLuint, index: GLuint, bufSize: GLsizei, length: *GLsizei, size: *GLsizei, type: *GLenum, name: *GLchar);
    func glGetUniformuiv(program: GLuint, location: GLint, params: *GLuint);
    func glGetVertexAttribIiv(index: GLuint, pname: GLenum, params: *GLint);
    func glGetVertexAttribIuiv(index: GLuint, pname: GLenum, params: *GLuint);
    func glIsEnabledi(cap: GLenum, index: GLuint)->GLboolean;
    func glTexParameterIiv(target: GLenum, pname: GLenum, params: const *GLint);
    func glTexParameterIuiv(target: GLenum, pname: GLenum, params: const *GLuint);
    func glTransformFeedbackVaryings(program: GLuint, count: GLsizei, varyings: *const *GLchar, bufferMode: GLenum);
    func glUniform1ui(location: GLint, v0: GLuint);
    func glUniform1uiv(location: GLint, count: GLsizei, value: const *GLuint);
    func glUniform2ui(location: GLint, v0: GLuint, v1: GLuint);
    func glUniform2uiv(location: GLint, count: GLsizei, value: const *GLuint);
    func glUniform3ui(location: GLint, v0: GLuint, v1: GLuint, v2: GLuint);
    func glUniform3uiv(location: GLint, count: GLsizei, value: const *GLuint);
    func glUniform4ui(location: GLint, v0: GLuint, v1: GLuint, v2: GLuint, v3: GLuint);
    func glUniform4uiv(location: GLint, count: GLsizei, value: const *GLuint);
    func glVertexAttribI1i(index: GLuint, v0: GLint);
    func glVertexAttribI1iv(index: GLuint, v0: const *GLint);
    func glVertexAttribI1ui(index: GLuint, v0: GLuint);
    func glVertexAttribI1uiv(index: GLuint, v0: const *GLuint);
    func glVertexAttribI2i(index: GLuint, v0: GLint, v1: GLint);
    func glVertexAttribI2iv(index: GLuint, v0: const *GLint);
    func glVertexAttribI2ui(index: GLuint, v0: GLuint, v1: GLuint);
    func glVertexAttribI2uiv(index: GLuint, v0: const *GLuint);
    func glVertexAttribI3i(index: GLuint, v0: GLint, v1: GLint, v2: GLint);
    func glVertexAttribI3iv(index: GLuint, v0: const *GLint);
    func glVertexAttribI3ui(index: GLuint, v0: GLuint, v1: GLuint, v2: GLuint);
    func glVertexAttribI3uiv(index: GLuint, v0: const *GLuint);
    func glVertexAttribI4bv(index: GLuint, v0: const *GLbyte);
    func glVertexAttribI4i(index: GLuint, v0: GLint, v1: GLint, v2: GLint, v3: GLint);
    func glVertexAttribI4iv(index: GLuint, v0: const *GLint);
    func glVertexAttribI4sv(index: GLuint, v0: const *GLshort);
    func glVertexAttribI4ubv(index: GLuint, v0: const *GLubyte);
    func glVertexAttribI4ui(index: GLuint, v0: GLuint, v1: GLuint, v2: GLuint, v3: GLuint);
    func glVertexAttribI4uiv(index: GLuint, v0: const *GLuint);
    func glVertexAttribI4usv(index: GLuint, v0: const *GLushort);
    func glVertexAttribIPointer(index: GLuint, size: GLint, type: GLenum, stride: GLsizei, pointer: const *void);
}

const GL_CLIP_DISTANCE0 = GL_CLIP_PLANE0
const GL_CLIP_DISTANCE1 = GL_CLIP_PLANE1
const GL_CLIP_DISTANCE2 = GL_CLIP_PLANE2
const GL_CLIP_DISTANCE3 = GL_CLIP_PLANE3
const GL_CLIP_DISTANCE4 = GL_CLIP_PLANE4
const GL_CLIP_DISTANCE5 = GL_CLIP_PLANE5
const GL_COMPARE_REF_TO_TEXTURE = GL_COMPARE_R_TO_TEXTURE_ARB
const GL_MAX_CLIP_DISTANCES = GL_MAX_CLIP_PLANES
const GL_MAX_VARYING_COMPONENTS = GL_MAX_VARYING_FLOATS
const GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT = 0x0001
const GL_MAJOR_VERSION = 0x821B
const GL_MINOR_VERSION = 0x821C
const GL_NUM_EXTENSIONS = 0x821D
const GL_CONTEXT_FLAGS = 0x821E
const GL_DEPTH_BUFFER = 0x8223
const GL_STENCIL_BUFFER = 0x8224
const GL_RGBA32F = 0x8814
const GL_RGB32F = 0x8815
const GL_RGBA16F = 0x881A
const GL_RGB16F = 0x881B
const GL_VERTEX_ATTRIB_ARRAY_INTEGER = 0x88FD
const GL_MAX_ARRAY_TEXTURE_LAYERS = 0x88FF
const GL_MIN_PROGRAM_TEXEL_OFFSET = 0x8904
const GL_MAX_PROGRAM_TEXEL_OFFSET = 0x8905
const GL_CLAMP_VERTEX_COLOR = 0x891A
const GL_CLAMP_FRAGMENT_COLOR = 0x891B
const GL_CLAMP_READ_COLOR = 0x891C
const GL_FIXED_ONLY = 0x891D
const GL_TEXTURE_RED_TYPE = 0x8C10
const GL_TEXTURE_GREEN_TYPE = 0x8C11
const GL_TEXTURE_BLUE_TYPE = 0x8C12
const GL_TEXTURE_ALPHA_TYPE = 0x8C13
const GL_TEXTURE_LUMINANCE_TYPE = 0x8C14
const GL_TEXTURE_INTENSITY_TYPE = 0x8C15
const GL_TEXTURE_DEPTH_TYPE = 0x8C16
const GL_TEXTURE_1D_ARRAY = 0x8C18
const GL_PROXY_TEXTURE_1D_ARRAY = 0x8C19
const GL_TEXTURE_2D_ARRAY = 0x8C1A
const GL_PROXY_TEXTURE_2D_ARRAY = 0x8C1B
const GL_TEXTURE_BINDING_1D_ARRAY = 0x8C1C
const GL_TEXTURE_BINDING_2D_ARRAY = 0x8C1D
const GL_R11F_G11F_B10F = 0x8C3A
const GL_UNSIGNED_INT_10F_11F_11F_REV = 0x8C3B
const GL_RGB9_E5 = 0x8C3D
const GL_UNSIGNED_INT_5_9_9_9_REV = 0x8C3E
const GL_TEXTURE_SHARED_SIZE = 0x8C3F
const GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH = 0x8C76
const GL_TRANSFORM_FEEDBACK_BUFFER_MODE = 0x8C7F
const GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS = 0x8C80
const GL_TRANSFORM_FEEDBACK_VARYINGS = 0x8C83
const GL_TRANSFORM_FEEDBACK_BUFFER_START = 0x8C84
const GL_TRANSFORM_FEEDBACK_BUFFER_SIZE = 0x8C85
const GL_PRIMITIVES_GENERATED = 0x8C87
const GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN = 0x8C88
const GL_RASTERIZER_DISCARD = 0x8C89
const GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS = 0x8C8A
const GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS = 0x8C8B
const GL_INTERLEAVED_ATTRIBS = 0x8C8C
const GL_SEPARATE_ATTRIBS = 0x8C8D
const GL_TRANSFORM_FEEDBACK_BUFFER = 0x8C8E
const GL_TRANSFORM_FEEDBACK_BUFFER_BINDING = 0x8C8F
const GL_RGBA32UI = 0x8D70
const GL_RGB32UI = 0x8D71
const GL_RGBA16UI = 0x8D76
const GL_RGB16UI = 0x8D77
const GL_RGBA8UI = 0x8D7C
const GL_RGB8UI = 0x8D7D
const GL_RGBA32I = 0x8D82
const GL_RGB32I = 0x8D83
const GL_RGBA16I = 0x8D88
const GL_RGB16I = 0x8D89
const GL_RGBA8I = 0x8D8E
const GL_RGB8I = 0x8D8F
const GL_RED_INTEGER = 0x8D94
const GL_GREEN_INTEGER = 0x8D95
const GL_BLUE_INTEGER = 0x8D96
const GL_ALPHA_INTEGER = 0x8D97
const GL_RGB_INTEGER = 0x8D98
const GL_RGBA_INTEGER = 0x8D99
const GL_BGR_INTEGER = 0x8D9A
const GL_BGRA_INTEGER = 0x8D9B
const GL_SAMPLER_1D_ARRAY = 0x8DC0
const GL_SAMPLER_2D_ARRAY = 0x8DC1
const GL_SAMPLER_1D_ARRAY_SHADOW = 0x8DC3
const GL_SAMPLER_2D_ARRAY_SHADOW = 0x8DC4
const GL_SAMPLER_CUBE_SHADOW = 0x8DC5
const GL_UNSIGNED_INT_VEC2 = 0x8DC6
const GL_UNSIGNED_INT_VEC3 = 0x8DC7
const GL_UNSIGNED_INT_VEC4 = 0x8DC8
const GL_INT_SAMPLER_1D = 0x8DC9
const GL_INT_SAMPLER_2D = 0x8DCA
const GL_INT_SAMPLER_3D = 0x8DCB
const GL_INT_SAMPLER_CUBE = 0x8DCC
const GL_INT_SAMPLER_1D_ARRAY = 0x8DCE
const GL_INT_SAMPLER_2D_ARRAY = 0x8DCF
const GL_UNSIGNED_INT_SAMPLER_1D = 0x8DD1
const GL_UNSIGNED_INT_SAMPLER_2D = 0x8DD2
const GL_UNSIGNED_INT_SAMPLER_3D = 0x8DD3
const GL_UNSIGNED_INT_SAMPLER_CUBE = 0x8DD4
const GL_UNSIGNED_INT_SAMPLER_1D_ARRAY = 0x8DD6
const GL_UNSIGNED_INT_SAMPLER_2D_ARRAY = 0x8DD7
const GL_QUERY_WAIT = 0x8E13
const GL_QUERY_NO_WAIT = 0x8E14
const GL_QUERY_BY_REGION_WAIT = 0x8E15
const GL_QUERY_BY_REGION_NO_WAIT = 0x8E16