#global public
using Swag

#[extension]
{
    func glActiveTexture(texture: GLenum);
    func glClientActiveTexture(texture: GLenum);
    func glCompressedTexImage1D(target: GLenum, level: GLint, internalformat: GLenum, width: GLsizei, border: GLint, imageSize: GLsizei, data: const *void);
    func glCompressedTexImage2D(target: GLenum, level: GLint, internalformat: GLenum, width: GLsizei, height: GLsizei, border: GLint, imageSize: GLsizei, data: const *void);
    func glCompressedTexImage3D(target: GLenum, level: GLint, internalformat: GLenum, width: GLsizei, height: GLsizei, depth: GLsizei, border: GLint, imageSize: GLsizei, data: const *void);
    func glCompressedTexSubImage1D(target: GLenum, level: GLint, xoffset: GLint, width: GLsizei, format: GLenum, imageSize: GLsizei, data: const *void);
    func glCompressedTexSubImage2D(target: GLenum, level: GLint, xoffset: GLint, yoffset: GLint, width: GLsizei, height: GLsizei, format: GLenum, imageSize: GLsizei, data: const *void);func glCompressedTexSubImage3D(target: GLenum, level: GLint, xoffset: GLint, yoffset: GLint, zoffset: GLint, width: GLsizei, height: GLsizei, depth: GLsizei, format: GLenum, imageSize: GLsizei, data: const *void);
    func glGetCompressedTexImage(target: GLenum, lod: GLint, img: *void);
    func glLoadTransposeMatrixd(m: const [16] GLdouble);
    func glLoadTransposeMatrixf(m: const [16] GLfloat);
    func glMultTransposeMatrixd(m: const [16] GLdouble);
    func glMultTransposeMatrixf(m: const [16] GLfloat);
    func glMultiTexCoord1d(target: GLenum, s: GLdouble);
    func glMultiTexCoord1dv(target: GLenum, v: const *GLdouble);
    func glMultiTexCoord1f(target: GLenum, s: GLfloat);
    func glMultiTexCoord1fv(target: GLenum, v: const *GLfloat);
    func glMultiTexCoord1i(target: GLenum, s: GLint);
    func glMultiTexCoord1iv(target: GLenum, v: const *GLint);
    func glMultiTexCoord1s(target: GLenum, s: GLshort);
    func glMultiTexCoord1sv(target: GLenum, v: const *GLshort);
    func glMultiTexCoord2d(target: GLenum, s: GLdouble, t: GLdouble);
    func glMultiTexCoord2dv(target: GLenum, v: const *GLdouble);
    func glMultiTexCoord2f(target: GLenum, s: GLfloat, t: GLfloat);
    func glMultiTexCoord2fv(target: GLenum, v: const *GLfloat);
    func glMultiTexCoord2i(target: GLenum, s: GLint, t: GLint);
    func glMultiTexCoord2iv(target: GLenum, v: const *GLint);
    func glMultiTexCoord2s(target: GLenum, s: GLshort, t: GLshort);
    func glMultiTexCoord2sv(target: GLenum, v: const *GLshort);
    func glMultiTexCoord3d(target: GLenum, s: GLdouble, t: GLdouble, r: GLdouble);
    func glMultiTexCoord3dv(target: GLenum, v: const *GLdouble);
    func glMultiTexCoord3f(target: GLenum, s: GLfloat, t: GLfloat, r: GLfloat);
    func glMultiTexCoord3fv(target: GLenum, v: const *GLfloat);
    func glMultiTexCoord3i(target: GLenum, s: GLint, t: GLint, r: GLint);
    func glMultiTexCoord3iv(target: GLenum, v: const *GLint);
    func glMultiTexCoord3s(target: GLenum, s: GLshort, t: GLshort, r: GLshort);
    func glMultiTexCoord3sv(target: GLenum, v: const *GLshort);
    func glMultiTexCoord4d(target: GLenum, s: GLdouble, t: GLdouble, r: GLdouble, q: GLdouble);
    func glMultiTexCoord4dv(target: GLenum, v: const *GLdouble);
    func glMultiTexCoord4f(target: GLenum, s: GLfloat, t: GLfloat, r: GLfloat, q: GLfloat);
    func glMultiTexCoord4fv(target: GLenum, v: const *GLfloat);
    func glMultiTexCoord4i(target: GLenum, s: GLint, t: GLint, r: GLint, q: GLint);
    func glMultiTexCoord4iv(target: GLenum, v: const *GLint);
    func glMultiTexCoord4s(target: GLenum, s: GLshort, t: GLshort, r: GLshort, q: GLshort);
    func glMultiTexCoord4sv(target: GLenum, v: const *GLshort);
    func glSampleCoverage(value: GLclampf, invert: GLboolean);
}

const GL_MULTISAMPLE = 0x809D
const GL_SAMPLE_ALPHA_TO_COVERAGE = 0x809E
const GL_SAMPLE_ALPHA_TO_ONE = 0x809F
const GL_SAMPLE_COVERAGE = 0x80A0
const GL_SAMPLE_BUFFERS = 0x80A8
const GL_SAMPLES = 0x80A9
const GL_SAMPLE_COVERAGE_VALUE = 0x80AA
const GL_SAMPLE_COVERAGE_INVERT = 0x80AB
const GL_CLAMP_TO_BORDER = 0x812D
const GL_TEXTURE0 = 0x84C0
const GL_TEXTURE1 = 0x84C1
const GL_TEXTURE2 = 0x84C2
const GL_TEXTURE3 = 0x84C3
const GL_TEXTURE4 = 0x84C4
const GL_TEXTURE5 = 0x84C5
const GL_TEXTURE6 = 0x84C6
const GL_TEXTURE7 = 0x84C7
const GL_TEXTURE8 = 0x84C8
const GL_TEXTURE9 = 0x84C9
const GL_TEXTURE10 = 0x84CA
const GL_TEXTURE11 = 0x84CB
const GL_TEXTURE12 = 0x84CC
const GL_TEXTURE13 = 0x84CD
const GL_TEXTURE14 = 0x84CE
const GL_TEXTURE15 = 0x84CF
const GL_TEXTURE16 = 0x84D0
const GL_TEXTURE17 = 0x84D1
const GL_TEXTURE18 = 0x84D2
const GL_TEXTURE19 = 0x84D3
const GL_TEXTURE20 = 0x84D4
const GL_TEXTURE21 = 0x84D5
const GL_TEXTURE22 = 0x84D6
const GL_TEXTURE23 = 0x84D7
const GL_TEXTURE24 = 0x84D8
const GL_TEXTURE25 = 0x84D9
const GL_TEXTURE26 = 0x84DA
const GL_TEXTURE27 = 0x84DB
const GL_TEXTURE28 = 0x84DC
const GL_TEXTURE29 = 0x84DD
const GL_TEXTURE30 = 0x84DE
const GL_TEXTURE31 = 0x84DF
const GL_ACTIVE_TEXTURE = 0x84E0
const GL_CLIENT_ACTIVE_TEXTURE = 0x84E1
const GL_MAX_TEXTURE_UNITS = 0x84E2
const GL_TRANSPOSE_MODELVIEW_MATRIX = 0x84E3
const GL_TRANSPOSE_PROJECTION_MATRIX = 0x84E4
const GL_TRANSPOSE_TEXTURE_MATRIX = 0x84E5
const GL_TRANSPOSE_COLOR_MATRIX = 0x84E6
const GL_SUBTRACT = 0x84E7
const GL_COMPRESSED_ALPHA = 0x84E9
const GL_COMPRESSED_LUMINANCE = 0x84EA
const GL_COMPRESSED_LUMINANCE_ALPHA = 0x84EB
const GL_COMPRESSED_INTENSITY = 0x84EC
const GL_COMPRESSED_RGB = 0x84ED
const GL_COMPRESSED_RGBA = 0x84EE
const GL_TEXTURE_COMPRESSION_HINT = 0x84EF
const GL_NORMAL_MAP = 0x8511
const GL_REFLECTION_MAP = 0x8512
const GL_TEXTURE_CUBE_MAP = 0x8513
const GL_TEXTURE_BINDING_CUBE_MAP = 0x8514
const GL_TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515
const GL_TEXTURE_CUBE_MAP_NEGATIVE_X = 0x8516
const GL_TEXTURE_CUBE_MAP_POSITIVE_Y = 0x8517
const GL_TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x8518
const GL_TEXTURE_CUBE_MAP_POSITIVE_Z = 0x8519
const GL_TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A
const GL_PROXY_TEXTURE_CUBE_MAP = 0x851B
const GL_MAX_CUBE_MAP_TEXTURE_SIZE = 0x851C
const GL_COMBINE = 0x8570
const GL_COMBINE_RGB = 0x8571
const GL_COMBINE_ALPHA = 0x8572
const GL_RGB_SCALE = 0x8573
const GL_ADD_SIGNED = 0x8574
const GL_INTERPOLATE = 0x8575
const GL_CONSTANT = 0x8576
const GL_PRIMARY_COLOR = 0x8577
const GL_PREVIOUS = 0x8578
const GL_SOURCE0_RGB = 0x8580
const GL_SOURCE1_RGB = 0x8581
const GL_SOURCE2_RGB = 0x8582
const GL_SOURCE0_ALPHA = 0x8588
const GL_SOURCE1_ALPHA = 0x8589
const GL_SOURCE2_ALPHA = 0x858A
const GL_OPERAND0_RGB = 0x8590
const GL_OPERAND1_RGB = 0x8591
const GL_OPERAND2_RGB = 0x8592
const GL_OPERAND0_ALPHA = 0x8598
const GL_OPERAND1_ALPHA = 0x8599
const GL_OPERAND2_ALPHA = 0x859A
const GL_TEXTURE_COMPRESSED_IMAGE_SIZE = 0x86A0
const GL_TEXTURE_COMPRESSED = 0x86A1
const GL_NUM_COMPRESSED_TEXTURE_FORMATS = 0x86A2
const GL_COMPRESSED_TEXTURE_FORMATS = 0x86A3
const GL_DOT3_RGB = 0x86AE
const GL_DOT3_RGBA = 0x86AF
const GL_MULTISAMPLE_BIT = 0x20000000
