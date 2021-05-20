#global public
using Swag

#[extension]
{
    func glBlendColor(red: GLclampf, green: GLclampf, blue: GLclampf, alpha: GLclampf);
    func glBlendEquation(mode: GLenum);
    func glBlendFuncSeparate(sfactorRGB: GLenum, dfactorRGB: GLenum, sfactorAlpha: GLenum, dfactorAlpha: GLenum);
    func glFogCoordPointer(type: GLenum, stride: GLsizei, pointer: const *void);
    func glFogCoordd(coord: GLdouble);
    func glFogCoorddv(coord: const *GLdouble);
    func glFogCoordf(coord: GLfloat);
    func glFogCoordfv(coord: const *GLfloat);
    func glMultiDrawArrays(mode: GLenum, first: const *GLint, count: const *GLsizei, drawcount: GLsizei);
    func glMultiDrawElements(mode: GLenum, count: const *GLsizei, type: GLenum, indices: const **void, drawcount: GLsizei);
    func glPointParameterf(pname: GLenum, param: GLfloat);
    func glPointParameterfv(pname: GLenum, params: const *GLfloat);
    func glPointParameteri(pname: GLenum, param: GLint);
    func glPointParameteriv(pname: GLenum, params: const *GLint);
    func glSecondaryColor3b(red: GLbyte, green: GLbyte, blue: GLbyte);
    func glSecondaryColor3bv(v: const *GLbyte);
    func glSecondaryColor3d(red: GLdouble, green: GLdouble, blue: GLdouble);
    func glSecondaryColor3dv(v: const *GLdouble);
    func glSecondaryColor3f(red: GLfloat, green: GLfloat, blue: GLfloat);
    func glSecondaryColor3fv(v: const *GLfloat);
    func glSecondaryColor3i(red: GLint, green: GLint, blue: GLint);
    func glSecondaryColor3iv(v: const *GLint);
    func glSecondaryColor3s(red: GLshort, green: GLshort, blue: GLshort);
    func glSecondaryColor3sv(v: const *GLshort);
    func glSecondaryColor3ub(red: GLubyte, green: GLubyte, blue: GLubyte);
    func glSecondaryColor3ubv(v: const *GLubyte);
    func glSecondaryColor3ui(red: GLuint, green: GLuint, blue: GLuint);
    func glSecondaryColor3uiv(v: const *GLuint);
    func glSecondaryColor3us(red: GLushort, green: GLushort, blue: GLushort);
    func glSecondaryColor3usv(v: const *GLushort);
    func glSecondaryColorPointer(size: GLint, type: GLenum, stride: GLsizei, pointer: const *void);
    func glWindowPos2d(x: GLdouble, y: GLdouble);
    func glWindowPos2dv(p: const *GLdouble);
    func glWindowPos2f(x: GLfloat, y: GLfloat);
    func glWindowPos2fv(p: const *GLfloat);
    func glWindowPos2i(x: GLint, y: GLint);
    func glWindowPos2iv(p: const *GLint);
    func glWindowPos2s(x: GLshort, y: GLshort);
    func glWindowPos2sv(p: const *GLshort);
    func glWindowPos3d(x: GLdouble, y: GLdouble, z: GLdouble);
    func glWindowPos3dv(p: const *GLdouble);
    func glWindowPos3f(x: GLfloat, y: GLfloat, z: GLfloat);
    func glWindowPos3fv(p: const *GLfloat);
    func glWindowPos3i(x: GLint, y: GLint, z: GLint);
    func glWindowPos3iv(p: const *GLint);
    func glWindowPos3s(x: GLshort, y: GLshort, z: GLshort);
    func glWindowPos3sv(p: const *GLshort);
}

const GL_BLEND_DST_RGB = 0x80C8
const GL_BLEND_SRC_RGB = 0x80C9
const GL_BLEND_DST_ALPHA = 0x80CA
const GL_BLEND_SRC_ALPHA = 0x80CB
const GL_POINT_SIZE_MIN = 0x8126
const GL_POINT_SIZE_MAX = 0x8127
const GL_POINT_FADE_THRESHOLD_SIZE = 0x8128
const GL_POINT_DISTANCE_ATTENUATION = 0x8129
const GL_GENERATE_MIPMAP = 0x8191
const GL_GENERATE_MIPMAP_HINT = 0x8192
const GL_DEPTH_COMPONENT16 = 0x81A5
const GL_DEPTH_COMPONENT24 = 0x81A6
const GL_DEPTH_COMPONENT32 = 0x81A7
const GL_MIRRORED_REPEAT = 0x8370
const GL_FOG_COORDINATE_SOURCE = 0x8450
const GL_FOG_COORDINATE = 0x8451
const GL_FRAGMENT_DEPTH = 0x8452
const GL_CURRENT_FOG_COORDINATE = 0x8453
const GL_FOG_COORDINATE_ARRAY_TYPE = 0x8454
const GL_FOG_COORDINATE_ARRAY_STRIDE = 0x8455
const GL_FOG_COORDINATE_ARRAY_POINTER = 0x8456
const GL_FOG_COORDINATE_ARRAY = 0x8457
const GL_COLOR_SUM = 0x8458
const GL_CURRENT_SECONDARY_COLOR = 0x8459
const GL_SECONDARY_COLOR_ARRAY_SIZE = 0x845A
const GL_SECONDARY_COLOR_ARRAY_TYPE = 0x845B
const GL_SECONDARY_COLOR_ARRAY_STRIDE = 0x845C
const GL_SECONDARY_COLOR_ARRAY_POINTER = 0x845D
const GL_SECONDARY_COLOR_ARRAY = 0x845E
const GL_MAX_TEXTURE_LOD_BIAS = 0x84FD
const GL_TEXTURE_FILTER_CONTROL = 0x8500
const GL_TEXTURE_LOD_BIAS = 0x8501
const GL_INCR_WRAP = 0x8507
const GL_DECR_WRAP = 0x8508
const GL_TEXTURE_DEPTH_SIZE = 0x884A
const GL_DEPTH_TEXTURE_MODE = 0x884B
const GL_TEXTURE_COMPARE_MODE = 0x884C
const GL_TEXTURE_COMPARE_FUNC = 0x884D
const GL_COMPARE_R_TO_TEXTURE = 0x884E