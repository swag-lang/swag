#public

alias GLbitfield    = u32
alias GLint         = s32
alias GLsizei       = s32
alias GLclampf      = f32

const GL_COLOR_BUFFER_BIT = 0x00004000

#[swag.foreign("opengl32")]
{
    func glClearColor(red: GLclampf, green: GLclampf, blue: GLclampf, alpha: GLclampf);
    func glClear(mask: GLbitfield);
    func glViewport(x: GLint, y: GLint, width: GLsizei, height: GLsizei);
}