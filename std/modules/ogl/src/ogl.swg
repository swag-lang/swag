#public

alias GLbitfield    = u32
alias GLint         = s32
alias GLsizei       = s32
alias GLclampf      = f32
alias GLenum        = s32
alias GLfloat       = f32

const GL_COLOR_BUFFER_BIT = 0x00004000

const GL_POINTS             = 0x0000's32
const GL_LINES              = 0x0001's32
const GL_LINE_LOOP          = 0x0002's32
const GL_LINE_STRIP         = 0x0003's32
const GL_TRIANGLES          = 0x0004's32
const GL_TRIANGLE_STRIP     = 0x0005's32
const GL_TRIANGLE_FAN       = 0x0006's32
const GL_QUADS              = 0x0007's32
const GL_QUAD_STRIP         = 0x0008's32
const GL_POLYGON            = 0x0009's32

#[swag.foreign("opengl32")]
{
    func glClearColor(red: GLclampf, green: GLclampf, blue: GLclampf, alpha: GLclampf);
    func glClear(mask: GLbitfield);
    func glViewport(x: GLint, y: GLint, width: GLsizei, height: GLsizei);
    func glEnd();
    func glFlush();
    func glBegin(mode: GLenum);
    func glColor3f(red: GLfloat, green: GLfloat, blue: GLfloat);
    func glVertex2i(x: GLint, y: GLint);
}