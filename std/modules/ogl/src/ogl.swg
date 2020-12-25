#public

alias GLbitfield = u32

const GL_COLOR_BUFFER_BIT = 0x00004000

#[swag.foreign("opengl32")]
{
    func glClear(mask: GLbitfield);
}