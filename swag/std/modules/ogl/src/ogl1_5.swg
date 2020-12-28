using swag

#[attributeUsage(AttributeUsage.Function)]
attr extension()

#[extension]
{
    func glGenBuffers(n: GLsizei, buffer: *GLuint);
    func glGenVertexArrays(n: GLsizei, arrays: *GLuint);
    func glBindVertexArray(array: GLuint);
}

const GL_ARRAY_BUFFER = 0x8892