#public

alias GLbitfield    = u32
alias GLint         = s32
alias GLsizei       = s32

const GL_COLOR_BUFFER_BIT = 0x00004000

#[swag.foreign("opengl32")]
{
    func glClear(mask: GLbitfield);
    func glViewport(x: GLint, y: GLint, width: GLsizei, height: GLsizei);
}