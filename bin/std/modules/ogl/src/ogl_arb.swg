#global public
using Swag

/* ----------------------------- GL_ARB_shadow ----------------------------- */

const GL_TEXTURE_COMPARE_MODE_ARB = 0x884C
const GL_TEXTURE_COMPARE_FUNC_ARB = 0x884D
const GL_COMPARE_R_TO_TEXTURE_ARB = 0x884E

/* ----------------------------- GL_ARB_imaging ---------------------------- */

const GL_CONSTANT_COLOR                      = 0x8001
const GL_ONE_MINUS_CONSTANT_COLOR            = 0x8002
const GL_CONSTANT_ALPHA                      = 0x8003
const GL_ONE_MINUS_CONSTANT_ALPHA            = 0x8004
const GL_BLEND_COLOR                         = 0x8005
const GL_FUNC_ADD                            = 0x8006
const GL_MIN                                 = 0x8007
const GL_MAX                                 = 0x8008
const GL_BLEND_EQUATION                      = 0x8009
const GL_FUNC_SUBTRACT                       = 0x800A
const GL_FUNC_REVERSE_SUBTRACT               = 0x800B
const GL_CONVOLUTION_1D                      = 0x8010
const GL_CONVOLUTION_2D                      = 0x8011
const GL_SEPARABLE_2D                        = 0x8012
const GL_CONVOLUTION_BORDER_MODE             = 0x8013
const GL_CONVOLUTION_FILTER_SCALE            = 0x8014
const GL_CONVOLUTION_FILTER_BIAS             = 0x8015
const GL_REDUCE                              = 0x8016
const GL_CONVOLUTION_FORMAT                  = 0x8017
const GL_CONVOLUTION_WIDTH                   = 0x8018
const GL_CONVOLUTION_HEIGHT                  = 0x8019
const GL_MAX_CONVOLUTION_WIDTH               = 0x801A
const GL_MAX_CONVOLUTION_HEIGHT              = 0x801B
const GL_POST_CONVOLUTION_RED_SCALE          = 0x801C
const GL_POST_CONVOLUTION_GREEN_SCALE        = 0x801D
const GL_POST_CONVOLUTION_BLUE_SCALE         = 0x801E
const GL_POST_CONVOLUTION_ALPHA_SCALE        = 0x801F
const GL_POST_CONVOLUTION_RED_BIAS           = 0x8020
const GL_POST_CONVOLUTION_GREEN_BIAS         = 0x8021
const GL_POST_CONVOLUTION_BLUE_BIAS          = 0x8022
const GL_POST_CONVOLUTION_ALPHA_BIAS         = 0x8023
const GL_HISTOGRAM                           = 0x8024
const GL_PROXY_HISTOGRAM                     = 0x8025
const GL_HISTOGRAM_WIDTH                     = 0x8026
const GL_HISTOGRAM_FORMAT                    = 0x8027
const GL_HISTOGRAM_RED_SIZE                  = 0x8028
const GL_HISTOGRAM_GREEN_SIZE                = 0x8029
const GL_HISTOGRAM_BLUE_SIZE                 = 0x802A
const GL_HISTOGRAM_ALPHA_SIZE                = 0x802B
const GL_HISTOGRAM_LUMINANCE_SIZE            = 0x802C
const GL_HISTOGRAM_SINK                      = 0x802D
const GL_MINMAX                              = 0x802E
const GL_MINMAX_FORMAT                       = 0x802F
const GL_MINMAX_SINK                         = 0x8030
const GL_TABLE_TOO_LARGE                     = 0x8031
const GL_COLOR_MATRIX                        = 0x80B1
const GL_COLOR_MATRIX_STACK_DEPTH            = 0x80B2
const GL_MAX_COLOR_MATRIX_STACK_DEPTH        = 0x80B3
const GL_POST_COLOR_MATRIX_RED_SCALE         = 0x80B4
const GL_POST_COLOR_MATRIX_GREEN_SCALE       = 0x80B5
const GL_POST_COLOR_MATRIX_BLUE_SCALE        = 0x80B6
const GL_POST_COLOR_MATRIX_ALPHA_SCALE       = 0x80B7
const GL_POST_COLOR_MATRIX_RED_BIAS          = 0x80B8
const GL_POST_COLOR_MATRIX_GREEN_BIAS        = 0x80B9
const GL_POST_COLOR_MATRIX_BLUE_BIAS         = 0x80BA
const GL_POST_COLOR_MATRIX_ALPHA_BIAS        = 0x80BB
const GL_COLOR_TABLE                         = 0x80D0
const GL_POST_CONVOLUTION_COLOR_TABLE        = 0x80D1
const GL_POST_COLOR_MATRIX_COLOR_TABLE       = 0x80D2
const GL_PROXY_COLOR_TABLE                   = 0x80D3
const GL_PROXY_POST_CONVOLUTION_COLOR_TABLE  = 0x80D4
const GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE = 0x80D5
const GL_COLOR_TABLE_SCALE                   = 0x80D6
const GL_COLOR_TABLE_BIAS                    = 0x80D7
const GL_COLOR_TABLE_FORMAT                  = 0x80D8
const GL_COLOR_TABLE_WIDTH                   = 0x80D9
const GL_COLOR_TABLE_RED_SIZE                = 0x80DA
const GL_COLOR_TABLE_GREEN_SIZE              = 0x80DB
const GL_COLOR_TABLE_BLUE_SIZE               = 0x80DC
const GL_COLOR_TABLE_ALPHA_SIZE              = 0x80DD
const GL_COLOR_TABLE_LUMINANCE_SIZE          = 0x80DE
const GL_COLOR_TABLE_INTENSITY_SIZE          = 0x80DF
const GL_IGNORE_BORDER                       = 0x8150
const GL_CONSTANT_BORDER                     = 0x8151
const GL_WRAP_BORDER                         = 0x8152
const GL_REPLICATE_BORDER                    = 0x8153
const GL_CONVOLUTION_BORDER_COLOR            = 0x8154

/* ------------------- GL_ARB_framebuffer_no_attachments ------------------- */

const GL_FRAMEBUFFER_DEFAULT_WIDTH                  = 0x9310
const GL_FRAMEBUFFER_DEFAULT_HEIGHT                 = 0x9311
const GL_FRAMEBUFFER_DEFAULT_LAYERS                 = 0x9312
const GL_FRAMEBUFFER_DEFAULT_SAMPLES                = 0x9313
const GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS = 0x9314
const GL_MAX_FRAMEBUFFER_WIDTH                      = 0x9315
const GL_MAX_FRAMEBUFFER_HEIGHT                     = 0x9316
const GL_MAX_FRAMEBUFFER_LAYERS                     = 0x9317
const GL_MAX_FRAMEBUFFER_SAMPLES                    = 0x9318

#[Extension, Swag.PlaceHolder]
{
    func glFramebufferParameteri(target: GLenum, pname: GLenum, param: GLint);
    func glGetFramebufferParameteriv(target: GLenum, pname: GLenum, params: *GLint);
    func glGetNamedFramebufferParameterivEXT(framebuffer: GLuint, pname: GLenum, params: *GLint);
    func glNamedFramebufferParameteriEXT(framebuffer: GLuint, pname: GLenum, param: GLint);
}

/* ----------------------- GL_ARB_framebuffer_object ----------------------- */

const GL_INVALID_FRAMEBUFFER_OPERATION                = 0x0506
const GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING        = 0x8210
const GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE        = 0x8211
const GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE              = 0x8212
const GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE            = 0x8213
const GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE             = 0x8214
const GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE            = 0x8215
const GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE            = 0x8216
const GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE          = 0x8217
const GL_FRAMEBUFFER_DEFAULT                          = 0x8218
const GL_FRAMEBUFFER_UNDEFINED                        = 0x8219
const GL_DEPTH_STENCIL_ATTACHMENT                     = 0x821A
const GL_INDEX                                        = 0x8222
const GL_MAX_RENDERBUFFER_SIZE                        = 0x84E8
const GL_DEPTH_STENCIL                                = 0x84F9
const GL_UNSIGNED_INT_24_8                            = 0x84FA
const GL_DEPTH24_STENCIL8                             = 0x88F0
const GL_TEXTURE_STENCIL_SIZE                         = 0x88F1
const GL_UNSIGNED_NORMALIZED                          = 0x8C17
const GL_DRAW_FRAMEBUFFER_BINDING                     = 0x8CA6
const GL_FRAMEBUFFER_BINDING                          = 0x8CA6
const GL_RENDERBUFFER_BINDING                         = 0x8CA7
const GL_READ_FRAMEBUFFER                             = 0x8CA8
const GL_DRAW_FRAMEBUFFER                             = 0x8CA9
const GL_READ_FRAMEBUFFER_BINDING                     = 0x8CAA
const GL_RENDERBUFFER_SAMPLES                         = 0x8CAB
const GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE           = 0x8CD0
const GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME           = 0x8CD1
const GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL         = 0x8CD2
const GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE = 0x8CD3
const GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER         = 0x8CD4
const GL_FRAMEBUFFER_COMPLETE                         = 0x8CD5
const GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT            = 0x8CD6
const GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT    = 0x8CD7
const GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER           = 0x8CDB
const GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER           = 0x8CDC
const GL_FRAMEBUFFER_UNSUPPORTED                      = 0x8CDD
const GL_MAX_COLOR_ATTACHMENTS                        = 0x8CDF
const GL_COLOR_ATTACHMENT0                            = 0x8CE0
const GL_COLOR_ATTACHMENT1                            = 0x8CE1
const GL_COLOR_ATTACHMENT2                            = 0x8CE2
const GL_COLOR_ATTACHMENT3                            = 0x8CE3
const GL_COLOR_ATTACHMENT4                            = 0x8CE4
const GL_COLOR_ATTACHMENT5                            = 0x8CE5
const GL_COLOR_ATTACHMENT6                            = 0x8CE6
const GL_COLOR_ATTACHMENT7                            = 0x8CE7
const GL_COLOR_ATTACHMENT8                            = 0x8CE8
const GL_COLOR_ATTACHMENT9                            = 0x8CE9
const GL_COLOR_ATTACHMENT10                           = 0x8CEA
const GL_COLOR_ATTACHMENT11                           = 0x8CEB
const GL_COLOR_ATTACHMENT12                           = 0x8CEC
const GL_COLOR_ATTACHMENT13                           = 0x8CED
const GL_COLOR_ATTACHMENT14                           = 0x8CEE
const GL_COLOR_ATTACHMENT15                           = 0x8CEF
const GL_DEPTH_ATTACHMENT                             = 0x8D00
const GL_STENCIL_ATTACHMENT                           = 0x8D20
const GL_FRAMEBUFFER                                  = 0x8D40
const GL_RENDERBUFFER                                 = 0x8D41
const GL_RENDERBUFFER_WIDTH                           = 0x8D42
const GL_RENDERBUFFER_HEIGHT                          = 0x8D43
const GL_RENDERBUFFER_INTERNAL_FORMAT                 = 0x8D44
const GL_STENCIL_INDEX1                               = 0x8D46
const GL_STENCIL_INDEX4                               = 0x8D47
const GL_STENCIL_INDEX8                               = 0x8D48
const GL_STENCIL_INDEX16                              = 0x8D49
const GL_RENDERBUFFER_RED_SIZE                        = 0x8D50
const GL_RENDERBUFFER_GREEN_SIZE                      = 0x8D51
const GL_RENDERBUFFER_BLUE_SIZE                       = 0x8D52
const GL_RENDERBUFFER_ALPHA_SIZE                      = 0x8D53
const GL_RENDERBUFFER_DEPTH_SIZE                      = 0x8D54
const GL_RENDERBUFFER_STENCIL_SIZE                    = 0x8D55
const GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE           = 0x8D56
const GL_MAX_SAMPLES                                  = 0x8D57

#[Extension, Swag.PlaceHolder]
{
    func glBindFramebuffer(target: GLenum, framebuffer: GLuint);
    func glBindRenderbuffer(target: GLenum, renderbuffer: GLuint);
    func glBlitFramebuffer(srcX0: GLint, srcY0: GLint, srcX1: GLint, srcY1: GLint, dstX0: GLint, dstY0: GLint, dstX1: GLint, dstY1: GLint, mask: GLbitfield, filter: GLenum);
    func glCheckFramebufferStatus(target: GLenum)->GLenum;
    func glDeleteFramebuffers(n: GLsizei, framebuffers: const *GLuint);
    func glDeleteRenderbuffers(n: GLsizei, renderbuffers: const *GLuint);
    func glFramebufferRenderbuffer(target: GLenum, attachment: GLenum, renderbuffertarget: GLenum, renderbuffer: GLuint);
    func glFramebufferTexture1D(target: GLenum, attachment: GLenum, textarget: GLenum, texture: GLuint, level: GLint);
    func glFramebufferTexture2D(target: GLenum, attachment: GLenum, textarget: GLenum, texture: GLuint, level: GLint);
    func glFramebufferTexture3D(target: GLenum, attachment: GLenum, textarget: GLenum, texture: GLuint, level: GLint, layer: GLint);
    func glFramebufferTextureLayer(target: GLenum, attachment: GLenum, texture: GLuint, level: GLint, layer: GLint);
    func glGenFramebuffers(n: GLsizei, framebuffers: *GLuint);
    func glGenRenderbuffers(n: GLsizei, renderbuffers: *GLuint);
    func glGenerateMipmap(target: GLenum);
    func glGetFramebufferAttachmentParameteriv(target: GLenum, attachment: GLenum, pname: GLenum, params: *GLint);
    func glGetRenderbufferParameteriv(target: GLenum, pname: GLenum, params: *GLint);
    func glIsFramebuffer(framebuffer: GLuint)->GLboolean;
    func glIsRenderbuffer(renderbuffer: GLuint)->GLboolean;
    func glRenderbufferStorage(target: GLenum, internalformat: GLenum, width: GLsizei, height: GLsizei);
    func glRenderbufferStorageMultisample(target: GLenum, samples: GLsizei, internalformat: GLenum, width: GLsizei, height: GLsizei);
}

/* ------------------------ GL_ARB_framebuffer_sRGB ------------------------ */

/* -------------------- GL_ARB_shading_language_include -------------------- */

const GL_SHADER_INCLUDE_ARB      = 0x8DAE
const GL_NAMED_STRING_LENGTH_ARB = 0x8DE9
const GL_NAMED_STRING_TYPE_ARB   = 0x8DEA

#[Extension, Swag.PlaceHolder]
{
    func glCompileShaderIncludeARB(shader: GLuint, count: GLsizei, path: const **GLchar, length: const *GLint);
    func glDeleteNamedStringARB(namelen: GLint, name: const *GLchar);
    func glGetNamedStringARB(namelen: GLint, name: const *GLchar, bufSize: GLsizei, stringlen: *GLint, str: *GLchar);
    func glGetNamedStringivARB(namelen: GLint, name: const *GLchar, pname: GLenum, params: *GLint);
    func glIsNamedStringARB(namelen: GLint, name: const *GLchar)->GLboolean;
    func glNamedStringARB(type: GLenum, namelen: GLint, name: const *GLchar, stringlen: GLint, str: const *GLchar);
}

const GL_FRAMEBUFFER_SRGB = 0x8DB9
