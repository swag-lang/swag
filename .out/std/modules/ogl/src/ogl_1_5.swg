#global public
using Swag

alias GLintptr   = int
alias GLsizeiptr = uint

#[extension]
{
    func glBeginQuery(target: GLenum, id: GLuint);
    func glBindBuffer(target: GLenum, buffer: GLuint);
    func glBufferData(target: GLenum, size: GLsizeiptr, data: const *void, usage: GLenum);
    func glBufferSubData(target: GLenum, offset: GLintptr, size: GLsizeiptr, data: const *void);
    func glDeleteBuffers(n: GLsizei, buffers: const *GLuint);
    func glDeleteQueries(n: GLsizei, ids: const *GLuint);
    func glEndQuery(target: GLenum);
    func glGenBuffers(n: GLsizei, buffers: *GLuint);
    func glGenQueries(n: GLsizei, ids: *GLuint);
    func glGetBufferParameteriv(target: GLenum, pname: GLenum, params: *GLint);
    func glGetBufferPointerv(target: GLenum, pname: GLenum, params: **void);
    func glGetBufferSubData(target: GLenum, offset: GLintptr, size: GLsizeiptr, data: *void);
    func glGetQueryObjectiv(id: GLuint, pname: GLenum, params: *GLint);
    func glGetQueryObjectuiv(id: GLuint, pname: GLenum, params: *GLuint);
    func glGetQueryiv(target: GLenum, pname: GLenum, params: *GLint);
    func glIsBuffer(buffer: GLuint)->GLboolean;
    func glIsQuery(id: GLuint)->GLboolean;
    func glMapBuffer(target: GLenum, access: GLenum)->*void;
    func glUnmapBuffer(target: GLenum)->GLboolean;
}

const GL_CURRENT_FOG_COORD = GL_CURRENT_FOG_COORDINATE
const GL_FOG_COORD = GL_FOG_COORDINATE
const GL_FOG_COORD_ARRAY = GL_FOG_COORDINATE_ARRAY
const GL_FOG_COORD_ARRAY_BUFFER_BINDING = GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING
const GL_FOG_COORD_ARRAY_POINTER = GL_FOG_COORDINATE_ARRAY_POINTER
const GL_FOG_COORD_ARRAY_STRIDE = GL_FOG_COORDINATE_ARRAY_STRIDE
const GL_FOG_COORD_ARRAY_TYPE = GL_FOG_COORDINATE_ARRAY_TYPE
const GL_FOG_COORD_SRC = GL_FOG_COORDINATE_SOURCE
const GL_SRC0_ALPHA = GL_SOURCE0_ALPHA
const GL_SRC0_RGB = GL_SOURCE0_RGB
const GL_SRC1_ALPHA = GL_SOURCE1_ALPHA
const GL_SRC1_RGB = GL_SOURCE1_RGB
const GL_SRC2_ALPHA = GL_SOURCE2_ALPHA
const GL_SRC2_RGB = GL_SOURCE2_RGB
const GL_BUFFER_SIZE = 0x8764
const GL_BUFFER_USAGE = 0x8765
const GL_QUERY_COUNTER_BITS = 0x8864
const GL_CURRENT_QUERY = 0x8865
const GL_QUERY_RESULT = 0x8866
const GL_QUERY_RESULT_AVAILABLE = 0x8867
const GL_ARRAY_BUFFER = 0x8892
const GL_ELEMENT_ARRAY_BUFFER = 0x8893
const GL_ARRAY_BUFFER_BINDING = 0x8894
const GL_ELEMENT_ARRAY_BUFFER_BINDING = 0x8895
const GL_VERTEX_ARRAY_BUFFER_BINDING = 0x8896
const GL_NORMAL_ARRAY_BUFFER_BINDING = 0x8897
const GL_COLOR_ARRAY_BUFFER_BINDING = 0x8898
const GL_INDEX_ARRAY_BUFFER_BINDING = 0x8899
const GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING = 0x889A
const GL_EDGE_FLAG_ARRAY_BUFFER_BINDING = 0x889B
const GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING = 0x889C
const GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING = 0x889D
const GL_WEIGHT_ARRAY_BUFFER_BINDING = 0x889E
const GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING = 0x889F
const GL_READ_ONLY = 0x88B8
const GL_WRITE_ONLY = 0x88B9
const GL_READ_WRITE = 0x88BA
const GL_BUFFER_ACCESS = 0x88BB
const GL_BUFFER_MAPPED = 0x88BC
const GL_BUFFER_MAP_POINTER = 0x88BD
const GL_STREAM_DRAW = 0x88E0
const GL_STREAM_READ = 0x88E1
const GL_STREAM_COPY = 0x88E2
const GL_STATIC_DRAW = 0x88E4
const GL_STATIC_READ = 0x88E5
const GL_STATIC_COPY = 0x88E6
const GL_DYNAMIC_DRAW = 0x88E8
const GL_DYNAMIC_READ = 0x88E9
const GL_DYNAMIC_COPY = 0x88EA
const GL_SAMPLES_PASSED = 0x8914