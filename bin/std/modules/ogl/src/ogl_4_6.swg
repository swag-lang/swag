#global public

#[Extension, Swag.PlaceHolder]
{
    func glMultiDrawArraysIndirectCount(mode: GLenum, indirect: const *GLvoid, drawcount: GLintptr, maxdrawcount: GLsizei, stride: GLsizei);
    func glMultiDrawElementsIndirectCount(mode: GLenum, type: GLenum, indirect: const *GLvoid, drawcount: GLintptr, maxdrawcount: GLsizei, stride: GLsizei);
    func glSpecializeShader(shader: GLuint, pEntryPoint: const *GLchar, numSpecializationConstants: GLuint, pConstantIndex: const *GLuint, pConstantValue: const *GLuint);
}

const GL_CONTEXT_FLAG_NO_ERROR_BIT          = 0x00000008
const GL_PARAMETER_BUFFER                   = 0x80EE
const GL_PARAMETER_BUFFER_BINDING           = 0x80EF
const GL_TRANSFORM_FEEDBACK_OVERFLOW        = 0x82EC
const GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW = 0x82ED
const GL_VERTICES_SUBMITTED                 = 0x82EE
const GL_PRIMITIVES_SUBMITTED               = 0x82EF
const GL_VERTEX_SHADER_INVOCATIONS          = 0x82F0
const GL_TESS_CONTROL_SHADER_PATCHES        = 0x82F1
const GL_TESS_EVALUATION_SHADER_INVOCATIONS = 0x82F2
const GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED = 0x82F3
const GL_FRAGMENT_SHADER_INVOCATIONS        = 0x82F4
const GL_COMPUTE_SHADER_INVOCATIONS         = 0x82F5
const GL_CLIPPING_INPUT_PRIMITIVES          = 0x82F6
const GL_CLIPPING_OUTPUT_PRIMITIVES         = 0x82F7
const GL_TEXTURE_MAX_ANISOTROPY             = 0x84FE
const GL_MAX_TEXTURE_MAX_ANISOTROPY         = 0x84FF
const GL_POLYGON_OFFSET_CLAMP               = 0x8E1B
const GL_SHADER_BINARY_FORMAT_SPIR_V        = 0x9551
const GL_SPIR_V_BINARY                      = 0x9552
const GL_SPIR_V_EXTENSIONS                  = 0x9553
const GL_NUM_SPIR_V_EXTENSIONS              = 0x9554
