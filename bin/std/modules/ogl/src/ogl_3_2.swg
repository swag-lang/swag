#global public

#[Extension, Swag.PlaceHolder]
{
    func glFramebufferTexture(target: GLenum, attachment: GLenum, texture: GLuint, level: GLint);
    func glGetBufferParameteri64v(target: GLenum, value: GLenum, data: *GLint64);
    func glGetInteger64i_v(pname: GLenum, index: GLuint, data: *GLint64);
}

const GL_CONTEXT_CORE_PROFILE_BIT             = 0x00000001
const GL_CONTEXT_COMPATIBILITY_PROFILE_BIT    = 0x00000002
const GL_LINES_ADJACENCY                      = 0x000A
const GL_LINE_STRIP_ADJACENCY                 = 0x000B
const GL_TRIANGLES_ADJACENCY                  = 0x000C
const GL_TRIANGLE_STRIP_ADJACENCY             = 0x000D
const GL_PROGRAM_POINT_SIZE                   = 0x8642
const GL_GEOMETRY_VERTICES_OUT                = 0x8916
const GL_GEOMETRY_INPUT_TYPE                  = 0x8917
const GL_GEOMETRY_OUTPUT_TYPE                 = 0x8918
const GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS     = 0x8C29
const GL_FRAMEBUFFER_ATTACHMENT_LAYERED       = 0x8DA7
const GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS = 0x8DA8
const GL_GEOMETRY_SHADER                      = 0x8DD9
const GL_MAX_GEOMETRY_UNIFORM_COMPONENTS      = 0x8DDF
const GL_MAX_GEOMETRY_OUTPUT_VERTICES         = 0x8DE0
const GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS = 0x8DE1
const GL_MAX_VERTEX_OUTPUT_COMPONENTS         = 0x9122
const GL_MAX_GEOMETRY_INPUT_COMPONENTS        = 0x9123
const GL_MAX_GEOMETRY_OUTPUT_COMPONENTS       = 0x9124
const GL_MAX_FRAGMENT_INPUT_COMPONENTS        = 0x9125
const GL_CONTEXT_PROFILE_MASK                 = 0x9126
