#global public
using Swag

/* ------------------------------ GL_KHR_debug ----------------------------- */

const GL_CONTEXT_FLAG_DEBUG_BIT           = 0x00000002
const GL_DEBUG_OUTPUT_SYNCHRONOUS         = 0x8242
const GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH = 0x8243
const GL_DEBUG_CALLBACK_FUNCTION          = 0x8244
const GL_DEBUG_CALLBACK_USER_PARAM        = 0x8245
const GL_DEBUG_SOURCE_API                 = 0x8246
const GL_DEBUG_SOURCE_WINDOW_SYSTEM       = 0x8247
const GL_DEBUG_SOURCE_SHADER_COMPILER     = 0x8248
const GL_DEBUG_SOURCE_THIRD_PARTY         = 0x8249
const GL_DEBUG_SOURCE_APPLICATION         = 0x824A
const GL_DEBUG_SOURCE_OTHER               = 0x824B
const GL_DEBUG_TYPE_ERROR                 = 0x824C
const GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR   = 0x824D
const GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR    = 0x824E
const GL_DEBUG_TYPE_PORTABILITY           = 0x824F
const GL_DEBUG_TYPE_PERFORMANCE           = 0x8250
const GL_DEBUG_TYPE_OTHER                 = 0x8251
const GL_DEBUG_TYPE_MARKER                = 0x8268
const GL_DEBUG_TYPE_PUSH_GROUP            = 0x8269
const GL_DEBUG_TYPE_POP_GROUP             = 0x826A
const GL_DEBUG_SEVERITY_NOTIFICATION      = 0x826B
const GL_MAX_DEBUG_GROUP_STACK_DEPTH      = 0x826C
const GL_DEBUG_GROUP_STACK_DEPTH          = 0x826D
const GL_BUFFER                           = 0x82E0
const GL_SHADER                           = 0x82E1
const GL_PROGRAM                          = 0x82E2
const GL_QUERY                            = 0x82E3
const GL_PROGRAM_PIPELINE                 = 0x82E4
const GL_SAMPLER                          = 0x82E6
const GL_DISPLAY_LIST                     = 0x82E7
const GL_MAX_LABEL_LENGTH                 = 0x82E8
const GL_MAX_DEBUG_MESSAGE_LENGTH         = 0x9143
const GL_MAX_DEBUG_LOGGED_MESSAGES        = 0x9144
const GL_DEBUG_LOGGED_MESSAGES            = 0x9145
const GL_DEBUG_SEVERITY_HIGH              = 0x9146
const GL_DEBUG_SEVERITY_MEDIUM            = 0x9147
const GL_DEBUG_SEVERITY_LOW               = 0x9148
const GL_DEBUG_OUTPUT                     = 0x92E0

//alias GLDEBUGPROC = func(source: GLenum, type: GLenum, id: GLuint, severity: GLenum, length: GLsizei, message: const *GLchar, userParam: const *void)
alias GLDEBUGPROC = func(GLenum, GLenum, GLuint, GLenum, GLsizei, const *GLchar, const *void)

#[Extension, Swag.PlaceHolder]
{
    func glDebugMessageCallback(callback: GLDEBUGPROC, userParam: const *void);
    func glDebugMessageControl(source: GLenum, type: GLenum, severity: GLenum, count: GLsizei, ids: const *GLuint, enabled: GLboolean);
    func glDebugMessageInsert(source: GLenum, type: GLenum, id: GLuint, severity: GLenum, length: GLsizei, buf: const *GLchar);
    func glGetDebugMessageLog(count: GLuint, bufSize: GLsizei, sources: *GLenum, types: *GLenum, ids: *GLuint, severities: *GLenum, lengths: *GLsizei, messageLog: *GLchar)->GLuint;
    func glGetObjectLabel(identifier: GLenum, name: GLuint, bufSize: GLsizei, length: *GLsizei, label: GLchar);
    func glGetObjectPtrLabel(ptr: *void, bufSize: GLsizei, length: *GLsizei, label: GLchar);
    func glObjectLabel(identifier: GLenum, name: GLuint, length: GLsizei, label: const *GLchar);
    func glObjectPtrLabel(ptr: *void, length: GLsizei, label: const *GLchar);
    func glPopDebugGroup();
    func glPushDebugGroup(source: GLenum, id: GLuint, length: GLsizei, message: const *GLchar);
}
