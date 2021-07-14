#global public

#[Extension]
{
    func glGetGraphicsResetStatus()->GLenum;
    func glGetnCompressedTexImage(target: GLenum, lod: GLint, bufSize: GLsizei, pixels: *GLvoid);
    func glGetnTexImage(tex: GLenum, level: GLint, format: GLenum, type: GLenum, bufSize: GLsizei, pixels: *GLvoid);
    func glGetnUniformdv(program: GLuint, location: GLint, bufSize: GLsizei, params: *GLdouble);
}

const GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT = 0x00000004