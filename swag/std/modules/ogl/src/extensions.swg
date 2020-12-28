using swag

#[attributeUsage(AttributeUsage.Function)]
attr extension()

#[extension]
{
    func glGenVertexArrays(n: GLsizei, arrays: *GLuint);
    func glBindVertexArray(array: GLuint);
}