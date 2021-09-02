#global public

#[Extension]
{
    func glAttachShader(program: GLuint, shader: GLuint);
    func glBindAttribLocation(program: GLuint, index: GLuint, name: const *GLchar);
    func glBlendEquationSeparate(modeRGB: GLenum, modeAlpha: GLenum);
    func glCompileShader(shader: GLuint);
    func glCreateProgram()->GLuint;
    func glCreateShader(type: GLenum)->GLuint;
    func glDeleteProgram(program: GLuint);
    func glDeleteShader(shader: GLuint);
    func glDetachShader(program: GLuint, shader: GLuint);
    func glDisableVertexAttribArray(index: GLuint);
    func glDrawBuffers(n: GLsizei, bufs: const *GLenum);
    func glEnableVertexAttribArray(index: GLuint);
    func glGetActiveAttrib(program: GLuint, index: GLuint, maxLength: GLsizei, length: *GLsizei, size: *GLint, type: *GLenum, name: *GLchar);
    func glGetActiveUniform(program: GLuint, index: GLuint, maxLength: GLsizei, length: *GLsizei, size: *GLint, type: *GLenum, name: *GLchar);
    func glGetAttachedShaders(program: GLuint, maxCount: GLsizei, count: *GLsizei, shaders: *GLuint);
    func glGetAttribLocation(program: GLuint, name: const *GLchar)->GLint;
    func glGetProgramInfoLog(program: GLuint, bufSize: GLsizei, length: *GLsizei, infoLog: *GLchar);
    func glGetProgramiv(program: GLuint, pname: GLenum, param: *GLint);
    func glGetShaderInfoLog(shader: GLuint, bufSize: GLsizei, length: *GLsizei, infoLog: *GLchar);
    func glGetShaderSource(obj: GLuint, maxLength: GLsizei, length: *GLsizei, source: *GLchar);
    func glGetShaderiv(shader: GLuint, pname: GLenum, param: *GLint);
    func glGetUniformLocation(program: GLuint, name: const *GLchar)->GLint;
    func glGetUniformfv(program: GLuint, location: GLint, params: *GLfloat);
    func glGetUniformiv(program: GLuint, location: GLint, params: *GLint);
    func glGetVertexAttribPointerv(index: GLuint, pname: GLenum, pointer: **void);
    func glGetVertexAttribdv(index: GLuint, pname: GLenum, params: *GLdouble);
    func glGetVertexAttribfv(index: GLuint, pname: GLenum, params: *GLfloat);
    func glGetVertexAttribiv(index: GLuint, pname: GLenum, params: *GLint);
    func glIsProgram(program: GLuint)->GLboolean;
    func glIsShader(shader: GLuint)->GLboolean;
    func glLinkProgram(program: GLuint);
    func glShaderSource(shader: GLuint, count: GLsizei, str: *const *GLchar, length: const *GLint);
    func glStencilFuncSeparate(frontfunc: GLenum, backfunc: GLenum, ref: GLint, mask: GLuint);
    func glStencilMaskSeparate(face: GLenum, mask: GLuint);
    func glStencilOpSeparate(face: GLenum, sfail: GLenum, dpfail: GLenum, dppass: GLenum);
    func glUniform1f(location: GLint, v0: GLfloat);
    func glUniform1fv(location: GLint, count: GLsizei, value: const *GLfloat);
    func glUniform1i(location: GLint, v0: GLint);
    func glUniform1iv(location: GLint, count: GLsizei, value: const *GLint);
    func glUniform2f(location: GLint, v0: GLfloat, v1: GLfloat);
    func glUniform2fv(location: GLint, count: GLsizei, value: const *GLfloat);
    func glUniform2i(location: GLint, v0: GLint, v1: GLint);
    func glUniform2iv(location: GLint, count: GLsizei, value: const *GLint);
    func glUniform3f(location: GLint, v0: GLfloat, v1: GLfloat, v2: GLfloat);
    func glUniform3fv(location: GLint, count: GLsizei, value: const *GLfloat);
    func glUniform3i(location: GLint, v0: GLint, v1: GLint, v2: GLint);
    func glUniform3iv(location: GLint, count: GLsizei, value: const *GLint);
    func glUniform4f(location: GLint, v0: GLfloat, v1: GLfloat, v2: GLfloat, v3: GLfloat);
    func glUniform4fv(location: GLint, count: GLsizei, value: const *GLfloat);
    func glUniform4i(location: GLint, v0: GLint, v1: GLint, v2: GLint, v3: GLint);
    func glUniform4iv(location: GLint, count: GLsizei, value: const *GLint);
    func glUniformMatrix2fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix3fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUniformMatrix4fv(location: GLint, count: GLsizei, transpose: GLboolean, value: const *GLfloat);
    func glUseProgram(program: GLuint);
    func glValidateProgram(program: GLuint);
    func glVertexAttrib1d(index: GLuint, x: GLdouble);
    func glVertexAttrib1dv(index: GLuint, v: const *GLdouble);
    func glVertexAttrib1f(index: GLuint, x: GLfloat);
    func glVertexAttrib1fv(index: GLuint, v: const *GLfloat);
    func glVertexAttrib1s(index: GLuint, x: GLshort);
    func glVertexAttrib1sv(index: GLuint, v: const *GLshort);
    func glVertexAttrib2d(index: GLuint, x: GLdouble, y: GLdouble);
    func glVertexAttrib2dv(index: GLuint, v: const *GLdouble);
    func glVertexAttrib2f(index: GLuint, x: GLfloat, y: GLfloat);
    func glVertexAttrib2fv(index: GLuint, v: const *GLfloat);
    func glVertexAttrib2s(index: GLuint, x: GLshort, y: GLshort);
    func glVertexAttrib2sv(index: GLuint, v: const *GLshort);
    func glVertexAttrib3d(index: GLuint, x: GLdouble, y: GLdouble, z: GLdouble);
    func glVertexAttrib3dv(index: GLuint, v: const *GLdouble);
    func glVertexAttrib3f(index: GLuint, x: GLfloat, y: GLfloat, z: GLfloat);
    func glVertexAttrib3fv(index: GLuint, v: const *GLfloat);
    func glVertexAttrib3s(index: GLuint, x: GLshort, y: GLshort, z: GLshort);
    func glVertexAttrib3sv(index: GLuint, v: const *GLshort);
    func glVertexAttrib4Nbv(index: GLuint, v: const *GLbyte);
    func glVertexAttrib4Niv(index: GLuint, v: const *GLint);
    func glVertexAttrib4Nsv(index: GLuint, v: const *GLshort);
    func glVertexAttrib4Nub(index: GLuint, x: GLubyte, y: GLubyte, z: GLubyte, w: GLubyte);
    func glVertexAttrib4Nubv(index: GLuint, v: const *GLubyte);
    func glVertexAttrib4Nuiv(index: GLuint, v: const *GLuint);
    func glVertexAttrib4Nusv(index: GLuint, v: const *GLushort);
    func glVertexAttrib4bv(index: GLuint, v: const *GLbyte);
    func glVertexAttrib4d(index: GLuint, x: GLdouble, y: GLdouble, z: GLdouble, w: GLdouble);
    func glVertexAttrib4dv(index: GLuint, v: const *GLdouble);
    func glVertexAttrib4f(index: GLuint, x: GLfloat, y: GLfloat, z: GLfloat, w: GLfloat);
    func glVertexAttrib4fv(index: GLuint, v: const *GLfloat);
    func glVertexAttrib4iv(index: GLuint, v: const *GLint);
    func glVertexAttrib4s(index: GLuint, x: GLshort, y: GLshort, z: GLshort, w: GLshort);
    func glVertexAttrib4sv(index: GLuint, v: const *GLshort);
    func glVertexAttrib4ubv(index: GLuint, v: const *GLubyte);
    func glVertexAttrib4uiv(index: GLuint, v: const *GLuint);
    func glVertexAttrib4usv(index: GLuint, v: const *GLushort);
    func glVertexAttribPointer(index: GLuint, size: GLint, type: GLenum, normalized: GLboolean, stride: GLsizei, pointer: const *void);
}

const GL_BLEND_EQUATION_RGB = GL_BLEND_EQUATION
const GL_VERTEX_ATTRIB_ARRAY_ENABLED = 0x8622
const GL_VERTEX_ATTRIB_ARRAY_SIZE = 0x8623
const GL_VERTEX_ATTRIB_ARRAY_STRIDE = 0x8624
const GL_VERTEX_ATTRIB_ARRAY_TYPE = 0x8625
const GL_CURRENT_VERTEX_ATTRIB = 0x8626
const GL_VERTEX_PROGRAM_POINT_SIZE = 0x8642
const GL_VERTEX_PROGRAM_TWO_SIDE = 0x8643
const GL_VERTEX_ATTRIB_ARRAY_POINTER = 0x8645
const GL_STENCIL_BACK_FUNC = 0x8800
const GL_STENCIL_BACK_FAIL = 0x8801
const GL_STENCIL_BACK_PASS_DEPTH_FAIL = 0x8802
const GL_STENCIL_BACK_PASS_DEPTH_PASS = 0x8803
const GL_MAX_DRAW_BUFFERS = 0x8824
const GL_DRAW_BUFFER0 = 0x8825
const GL_DRAW_BUFFER1 = 0x8826
const GL_DRAW_BUFFER2 = 0x8827
const GL_DRAW_BUFFER3 = 0x8828
const GL_DRAW_BUFFER4 = 0x8829
const GL_DRAW_BUFFER5 = 0x882A
const GL_DRAW_BUFFER6 = 0x882B
const GL_DRAW_BUFFER7 = 0x882C
const GL_DRAW_BUFFER8 = 0x882D
const GL_DRAW_BUFFER9 = 0x882E
const GL_DRAW_BUFFER10 = 0x882F
const GL_DRAW_BUFFER11 = 0x8830
const GL_DRAW_BUFFER12 = 0x8831
const GL_DRAW_BUFFER13 = 0x8832
const GL_DRAW_BUFFER14 = 0x8833
const GL_DRAW_BUFFER15 = 0x8834
const GL_BLEND_EQUATION_ALPHA = 0x883D
const GL_POINT_SPRITE = 0x8861
const GL_COORD_REPLACE = 0x8862
const GL_MAX_VERTEX_ATTRIBS = 0x8869
const GL_VERTEX_ATTRIB_ARRAY_NORMALIZED = 0x886A
const GL_MAX_TEXTURE_COORDS = 0x8871
const GL_MAX_TEXTURE_IMAGE_UNITS = 0x8872
const GL_FRAGMENT_SHADER = 0x8B30
const GL_VERTEX_SHADER = 0x8B31
const GL_MAX_FRAGMENT_UNIFORM_COMPONENTS = 0x8B49
const GL_MAX_VERTEX_UNIFORM_COMPONENTS = 0x8B4A
const GL_MAX_VARYING_FLOATS = 0x8B4B
const GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 0x8B4C
const GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D
const GL_SHADER_TYPE = 0x8B4F
const GL_FLOAT_VEC2 = 0x8B50
const GL_FLOAT_VEC3 = 0x8B51
const GL_FLOAT_VEC4 = 0x8B52
const GL_INT_VEC2 = 0x8B53
const GL_INT_VEC3 = 0x8B54
const GL_INT_VEC4 = 0x8B55
const GL_BOOL = 0x8B56
const GL_BOOL_VEC2 = 0x8B57
const GL_BOOL_VEC3 = 0x8B58
const GL_BOOL_VEC4 = 0x8B59
const GL_FLOAT_MAT2 = 0x8B5A
const GL_FLOAT_MAT3 = 0x8B5B
const GL_FLOAT_MAT4 = 0x8B5C
const GL_SAMPLER_1D = 0x8B5D
const GL_SAMPLER_2D = 0x8B5E
const GL_SAMPLER_3D = 0x8B5F
const GL_SAMPLER_CUBE = 0x8B60
const GL_SAMPLER_1D_SHADOW = 0x8B61
const GL_SAMPLER_2D_SHADOW = 0x8B62
const GL_DELETE_STATUS = 0x8B80
const GL_COMPILE_STATUS = 0x8B81
const GL_LINK_STATUS = 0x8B82
const GL_VALIDATE_STATUS = 0x8B83
const GL_INFO_LOG_LENGTH = 0x8B84
const GL_ATTACHED_SHADERS = 0x8B85
const GL_ACTIVE_UNIFORMS = 0x8B86
const GL_ACTIVE_UNIFORM_MAX_LENGTH = 0x8B87
const GL_SHADER_SOURCE_LENGTH = 0x8B88
const GL_ACTIVE_ATTRIBUTES = 0x8B89
const GL_ACTIVE_ATTRIBUTE_MAX_LENGTH = 0x8B8A
const GL_FRAGMENT_SHADER_DERIVATIVE_HINT = 0x8B8B
const GL_SHADING_LANGUAGE_VERSION = 0x8B8C
const GL_CURRENT_PROGRAM = 0x8B8D
const GL_POINT_SPRITE_COORD_ORIGIN = 0x8CA0
const GL_LOWER_LEFT = 0x8CA1
const GL_UPPER_LEFT = 0x8CA2
const GL_STENCIL_BACK_REF = 0x8CA3
const GL_STENCIL_BACK_VALUE_MASK = 0x8CA4
const GL_STENCIL_BACK_WRITEMASK = 0x8CA5
