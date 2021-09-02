#global public

#[Extension]
{
    func glDrawArraysInstanced(mode: GLenum, first: GLint, count: GLsizei, primcount: GLsizei);
    func glDrawElementsInstanced(mode: GLenum, count: GLsizei, type: GLenum, indices: const *void, primcount: GLsizei);
    func glPrimitiveRestartIndex(buffer: GLuint);
    func glTexBuffer(target: GLenum, internalFormat: GLenum, buffer: GLuint);
}

const GL_TEXTURE_RECTANGLE = 0x84F5
const GL_TEXTURE_BINDING_RECTANGLE = 0x84F6
const GL_PROXY_TEXTURE_RECTANGLE = 0x84F7
const GL_MAX_RECTANGLE_TEXTURE_SIZE = 0x84F8
const GL_SAMPLER_2D_RECT = 0x8B63
const GL_SAMPLER_2D_RECT_SHADOW = 0x8B64
const GL_TEXTURE_BUFFER = 0x8C2A
const GL_MAX_TEXTURE_BUFFER_SIZE = 0x8C2B
const GL_TEXTURE_BINDING_BUFFER = 0x8C2C
const GL_TEXTURE_BUFFER_DATA_STORE_BINDING = 0x8C2D
const GL_TEXTURE_BUFFER_FORMAT = 0x8C2E
const GL_SAMPLER_BUFFER = 0x8DC2
const GL_INT_SAMPLER_2D_RECT = 0x8DCD
const GL_INT_SAMPLER_BUFFER = 0x8DD0
const GL_UNSIGNED_INT_SAMPLER_2D_RECT = 0x8DD5
const GL_UNSIGNED_INT_SAMPLER_BUFFER = 0x8DD8
const GL_RED_SNORM = 0x8F90
const GL_RG_SNORM = 0x8F91
const GL_RGB_SNORM = 0x8F92
const GL_RGBA_SNORM = 0x8F93
const GL_R8_SNORM = 0x8F94
const GL_RG8_SNORM = 0x8F95
const GL_RGB8_SNORM = 0x8F96
const GL_RGBA8_SNORM = 0x8F97
const GL_R16_SNORM = 0x8F98
const GL_RG16_SNORM = 0x8F99
const GL_RGB16_SNORM = 0x8F9A
const GL_RGBA16_SNORM = 0x8F9B
const GL_SIGNED_NORMALIZED = 0x8F9C
const GL_PRIMITIVE_RESTART = 0x8F9D
const GL_PRIMITIVE_RESTART_INDEX = 0x8F9E
const GL_BUFFER_ACCESS_FLAGS = 0x911F
const GL_BUFFER_MAP_LENGTH = 0x9120
const GL_BUFFER_MAP_OFFSET = 0x9121