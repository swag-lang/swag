#global export

alias GLenum        = u32
alias GLboolean     = u8
alias GLbitfield    = u32
alias GLbyte        = s8
alias GLshort       = s16
alias GLint         = s32
alias GLsizei       = s32
alias GLubyte       = u8
alias GLushort      = u16
alias GLuint        = u32
alias GLfloat       = f32
alias GLclampf      = f32
alias GLdouble      = f64
alias GLclampd      = f64
alias GLvoid        = void

/* AccumOp */
const GL_ACCUM                          = 0x0100
const GL_LOAD                           = 0x0101
const GL_RETURN                         = 0x0102
const GL_MULT                           = 0x0103
const GL_ADD                            = 0x0104

/* AlphaFunction */
const GL_NEVER                          = 0x0200
const GL_LESS                           = 0x0201
const GL_EQUAL                          = 0x0202
const GL_LEQUAL                         = 0x0203
const GL_GREATER                        = 0x0204
const GL_NOTEQUAL                       = 0x0205
const GL_GEQUAL                         = 0x0206
const GL_ALWAYS                         = 0x0207

/* AttribMask */
const GL_CURRENT_BIT                    = 0x00000001
const GL_POINT_BIT                      = 0x00000002
const GL_LINE_BIT                       = 0x00000004
const GL_POLYGON_BIT                    = 0x00000008
const GL_POLYGON_STIPPLE_BIT            = 0x00000010
const GL_PIXEL_MODE_BIT                 = 0x00000020
const GL_LIGHTING_BIT                   = 0x00000040
const GL_FOG_BIT                        = 0x00000080
const GL_DEPTH_BUFFER_BIT               = 0x00000100
const GL_ACCUM_BUFFER_BIT               = 0x00000200
const GL_STENCIL_BUFFER_BIT             = 0x00000400
const GL_VIEWPORT_BIT                   = 0x00000800
const GL_TRANSFORM_BIT                  = 0x00001000
const GL_ENABLE_BIT                     = 0x00002000
const GL_COLOR_BUFFER_BIT               = 0x00004000
const GL_HINT_BIT                       = 0x00008000
const GL_EVAL_BIT                       = 0x00010000
const GL_LIST_BIT                       = 0x00020000
const GL_TEXTURE_BIT                    = 0x00040000
const GL_SCISSOR_BIT                    = 0x00080000
const GL_ALL_ATTRIB_BITS                = 0x000fffff

/* BeginMode */
const GL_POINTS             = 0x0000
const GL_LINES              = 0x0001
const GL_LINE_LOOP          = 0x0002
const GL_LINE_STRIP         = 0x0003
const GL_TRIANGLES          = 0x0004
const GL_TRIANGLE_STRIP     = 0x0005
const GL_TRIANGLE_FAN       = 0x0006
const GL_QUADS              = 0x0007
const GL_QUAD_STRIP         = 0x0008
const GL_POLYGON            = 0x0009

/* BlendingFactorDest */
const GL_ZERO                   = 0
const GL_ONE                    = 1
const GL_SRC_COLOR              = 0x0300
const GL_ONE_MINUS_SRC_COLOR    = 0x0301
const GL_SRC_ALPHA              = 0x0302
const GL_ONE_MINUS_SRC_ALPHA    = 0x0303
const GL_DST_ALPHA              = 0x0304
const GL_ONE_MINUS_DST_ALPHA    = 0x0305

/* BlendingFactorSrc */
const GL_DST_COLOR              = 0x0306
const GL_ONE_MINUS_DST_COLOR    = 0x0307
const GL_SRC_ALPHA_SATURATE     = 0x0308

/* Boolean */
const GL_TRUE   = 1
const GL_FALSE  = 0

/* ClipPlaneName */
const GL_CLIP_PLANE0 = 0x3000
const GL_CLIP_PLANE1 = 0x3001
const GL_CLIP_PLANE2 = 0x3002
const GL_CLIP_PLANE3 = 0x3003
const GL_CLIP_PLANE4 = 0x3004
const GL_CLIP_PLANE5 = 0x3005

/* DataType */
const GL_BYTE            = 0x1400
const GL_UNSIGNED_BYTE   = 0x1401
const GL_SHORT           = 0x1402
const GL_UNSIGNED_SHORT  = 0x1403
const GL_INT             = 0x1404
const GL_UNSIGNED_INT    = 0x1405
const GL_FLOAT           = 0x1406
const GL_2_BYTES         = 0x1407
const GL_3_BYTES         = 0x1408
const GL_4_BYTES         = 0x1409
const GL_DOUBLE          = 0x140A

/* DrawBufferMode */
const GL_NONE            = 0
const GL_FRONT_LEFT      = 0x0400
const GL_FRONT_RIGHT     = 0x0401
const GL_BACK_LEFT       = 0x0402
const GL_BACK_RIGHT      = 0x0403
const GL_FRONT           = 0x0404
const GL_BACK            = 0x0405
const GL_LEFT            = 0x0406
const GL_RIGHT           = 0x0407
const GL_FRONT_AND_BACK  = 0x0408
const GL_AUX0            = 0x0409
const GL_AUX1            = 0x040A
const GL_AUX2            = 0x040B
const GL_AUX3            = 0x040C

/* ErrorCode */
const GL_NO_ERROR                       = 0
const GL_INVALID_ENUM                   = 0x0500
const GL_INVALID_VALUE                  = 0x0501
const GL_INVALID_OPERATION              = 0x0502
const GL_STACK_OVERFLOW                 = 0x0503
const GL_STACK_UNDERFLOW                = 0x0504
const GL_OUT_OF_MEMORY                  = 0x0505

/* FeedBackMode */
const GL_2D                             = 0x0600
const GL_3D                             = 0x0601
const GL_3D_COLOR                       = 0x0602
const GL_3D_COLOR_TEXTURE               = 0x0603
const GL_4D_COLOR_TEXTURE               = 0x0604

/* FeedBackToken */
const GL_PASS_THROUGH_TOKEN             = 0x0700
const GL_POINT_TOKEN                    = 0x0701
const GL_LINE_TOKEN                     = 0x0702
const GL_POLYGON_TOKEN                  = 0x0703
const GL_BITMAP_TOKEN                   = 0x0704
const GL_DRAW_PIXEL_TOKEN               = 0x0705
const GL_COPY_PIXEL_TOKEN               = 0x0706
const GL_LINE_RESET_TOKEN               = 0x0707

/* FogMode */
const GL_EXP                            = 0x0800
const GL_EXP2                           = 0x0801

/* FrontFaceDirection */
const GL_CW                             = 0x0900
const GL_CCW                            = 0x0901

/* GetMapTarget */
const GL_COEFF                          = 0x0A00
const GL_ORDER                          = 0x0A01
const GL_DOMAIN                         = 0x0A02

/* GetTarget */
const GL_CURRENT_COLOR                  = 0x0B00
const GL_CURRENT_INDEX                  = 0x0B01
const GL_CURRENT_NORMAL                 = 0x0B02
const GL_CURRENT_TEXTURE_COORDS         = 0x0B03
const GL_CURRENT_RASTER_COLOR           = 0x0B04
const GL_CURRENT_RASTER_INDEX           = 0x0B05
const GL_CURRENT_RASTER_TEXTURE_COORDS  = 0x0B06
const GL_CURRENT_RASTER_POSITION        = 0x0B07
const GL_CURRENT_RASTER_POSITION_VALID  = 0x0B08
const GL_CURRENT_RASTER_DISTANCE        = 0x0B09
const GL_POINT_SMOOTH                   = 0x0B10
const GL_POINT_SIZE                     = 0x0B11
const GL_POINT_SIZE_RANGE               = 0x0B12
const GL_POINT_SIZE_GRANULARITY         = 0x0B13
const GL_LINE_SMOOTH                    = 0x0B20
const GL_LINE_WIDTH                     = 0x0B21
const GL_LINE_WIDTH_RANGE               = 0x0B22
const GL_LINE_WIDTH_GRANULARITY         = 0x0B23
const GL_LINE_STIPPLE                   = 0x0B24
const GL_LINE_STIPPLE_PATTERN           = 0x0B25
const GL_LINE_STIPPLE_REPEAT            = 0x0B26
const GL_LIST_MODE                      = 0x0B30
const GL_MAX_LIST_NESTING               = 0x0B31
const GL_LIST_BASE                      = 0x0B32
const GL_LIST_INDEX                     = 0x0B33
const GL_POLYGON_MODE                   = 0x0B40
const GL_POLYGON_SMOOTH                 = 0x0B41
const GL_POLYGON_STIPPLE                = 0x0B42
const GL_EDGE_FLAG                      = 0x0B43
const GL_CULL_FACE                      = 0x0B44
const GL_CULL_FACE_MODE                 = 0x0B45
const GL_FRONT_FACE                     = 0x0B46
const GL_LIGHTING                       = 0x0B50
const GL_LIGHT_MODEL_LOCAL_VIEWER       = 0x0B51
const GL_LIGHT_MODEL_TWO_SIDE           = 0x0B52
const GL_LIGHT_MODEL_AMBIENT            = 0x0B53
const GL_SHADE_MODEL                    = 0x0B54
const GL_COLOR_MATERIAL_FACE            = 0x0B55
const GL_COLOR_MATERIAL_PARAMETER       = 0x0B56
const GL_COLOR_MATERIAL                 = 0x0B57
const GL_FOG                            = 0x0B60
const GL_FOG_INDEX                      = 0x0B61
const GL_FOG_DENSITY                    = 0x0B62
const GL_FOG_START                      = 0x0B63
const GL_FOG_END                        = 0x0B64
const GL_FOG_MODE                       = 0x0B65
const GL_FOG_COLOR                      = 0x0B66
const GL_DEPTH_RANGE                    = 0x0B70
const GL_DEPTH_TEST                     = 0x0B71
const GL_DEPTH_WRITEMASK                = 0x0B72
const GL_DEPTH_CLEAR_VALUE              = 0x0B73
const GL_DEPTH_FUNC                     = 0x0B74
const GL_ACCUM_CLEAR_VALUE              = 0x0B80
const GL_STENCIL_TEST                   = 0x0B90
const GL_STENCIL_CLEAR_VALUE            = 0x0B91
const GL_STENCIL_FUNC                   = 0x0B92
const GL_STENCIL_VALUE_MASK             = 0x0B93
const GL_STENCIL_FAIL                   = 0x0B94
const GL_STENCIL_PASS_DEPTH_FAIL        = 0x0B95
const GL_STENCIL_PASS_DEPTH_PASS        = 0x0B96
const GL_STENCIL_REF                    = 0x0B97
const GL_STENCIL_WRITEMASK              = 0x0B98
const GL_MATRIX_MODE                    = 0x0BA0
const GL_NORMALIZE                      = 0x0BA1
const GL_VIEWPORT                       = 0x0BA2
const GL_MODELVIEW_STACK_DEPTH          = 0x0BA3
const GL_PROJECTION_STACK_DEPTH         = 0x0BA4
const GL_TEXTURE_STACK_DEPTH            = 0x0BA5
const GL_MODELVIEW_MATRIX               = 0x0BA6
const GL_PROJECTION_MATRIX              = 0x0BA7
const GL_TEXTURE_MATRIX                 = 0x0BA8
const GL_ATTRIB_STACK_DEPTH             = 0x0BB0
const GL_CLIENT_ATTRIB_STACK_DEPTH      = 0x0BB1
const GL_ALPHA_TEST                     = 0x0BC0
const GL_ALPHA_TEST_FUNC                = 0x0BC1
const GL_ALPHA_TEST_REF                 = 0x0BC2
const GL_DITHER                         = 0x0BD0
const GL_BLEND_DST                      = 0x0BE0
const GL_BLEND_SRC                      = 0x0BE1
const GL_BLEND                          = 0x0BE2
const GL_LOGIC_OP_MODE                  = 0x0BF0
const GL_INDEX_LOGIC_OP                 = 0x0BF1
const GL_COLOR_LOGIC_OP                 = 0x0BF2
const GL_AUX_BUFFERS                    = 0x0C00
const GL_DRAW_BUFFER                    = 0x0C01
const GL_READ_BUFFER                    = 0x0C02
const GL_SCISSOR_BOX                    = 0x0C10
const GL_SCISSOR_TEST                   = 0x0C11
const GL_INDEX_CLEAR_VALUE              = 0x0C20
const GL_INDEX_WRITEMASK                = 0x0C21
const GL_COLOR_CLEAR_VALUE              = 0x0C22
const GL_COLOR_WRITEMASK                = 0x0C23
const GL_INDEX_MODE                     = 0x0C30
const GL_RGBA_MODE                      = 0x0C31
const GL_DOUBLEBUFFER                   = 0x0C32
const GL_STEREO                         = 0x0C33
const GL_RENDER_MODE                    = 0x0C40
const GL_PERSPECTIVE_CORRECTION_HINT    = 0x0C50
const GL_POINT_SMOOTH_HINT              = 0x0C51
const GL_LINE_SMOOTH_HINT               = 0x0C52
const GL_POLYGON_SMOOTH_HINT            = 0x0C53
const GL_FOG_HINT                       = 0x0C54
const GL_TEXTURE_GEN_S                  = 0x0C60
const GL_TEXTURE_GEN_T                  = 0x0C61
const GL_TEXTURE_GEN_R                  = 0x0C62
const GL_TEXTURE_GEN_Q                  = 0x0C63
const GL_PIXEL_MAP_I_TO_I               = 0x0C70
const GL_PIXEL_MAP_S_TO_S               = 0x0C71
const GL_PIXEL_MAP_I_TO_R               = 0x0C72
const GL_PIXEL_MAP_I_TO_G               = 0x0C73
const GL_PIXEL_MAP_I_TO_B               = 0x0C74
const GL_PIXEL_MAP_I_TO_A               = 0x0C75
const GL_PIXEL_MAP_R_TO_R               = 0x0C76
const GL_PIXEL_MAP_G_TO_G               = 0x0C77
const GL_PIXEL_MAP_B_TO_B               = 0x0C78
const GL_PIXEL_MAP_A_TO_A               = 0x0C79
const GL_PIXEL_MAP_I_TO_I_SIZE          = 0x0CB0
const GL_PIXEL_MAP_S_TO_S_SIZE          = 0x0CB1
const GL_PIXEL_MAP_I_TO_R_SIZE          = 0x0CB2
const GL_PIXEL_MAP_I_TO_G_SIZE          = 0x0CB3
const GL_PIXEL_MAP_I_TO_B_SIZE          = 0x0CB4
const GL_PIXEL_MAP_I_TO_A_SIZE          = 0x0CB5
const GL_PIXEL_MAP_R_TO_R_SIZE          = 0x0CB6
const GL_PIXEL_MAP_G_TO_G_SIZE          = 0x0CB7
const GL_PIXEL_MAP_B_TO_B_SIZE          = 0x0CB8
const GL_PIXEL_MAP_A_TO_A_SIZE          = 0x0CB9
const GL_UNPACK_SWAP_BYTES              = 0x0CF0
const GL_UNPACK_LSB_FIRST               = 0x0CF1
const GL_UNPACK_ROW_LENGTH              = 0x0CF2
const GL_UNPACK_SKIP_ROWS               = 0x0CF3
const GL_UNPACK_SKIP_PIXELS             = 0x0CF4
const GL_UNPACK_ALIGNMENT               = 0x0CF5
const GL_PACK_SWAP_BYTES                = 0x0D00
const GL_PACK_LSB_FIRST                 = 0x0D01
const GL_PACK_ROW_LENGTH                = 0x0D02
const GL_PACK_SKIP_ROWS                 = 0x0D03
const GL_PACK_SKIP_PIXELS               = 0x0D04
const GL_PACK_ALIGNMENT                 = 0x0D05
const GL_MAP_COLOR                      = 0x0D10
const GL_MAP_STENCIL                    = 0x0D11
const GL_INDEX_SHIFT                    = 0x0D12
const GL_INDEX_OFFSET                   = 0x0D13
const GL_RED_SCALE                      = 0x0D14
const GL_RED_BIAS                       = 0x0D15
const GL_ZOOM_X                         = 0x0D16
const GL_ZOOM_Y                         = 0x0D17
const GL_GREEN_SCALE                    = 0x0D18
const GL_GREEN_BIAS                     = 0x0D19
const GL_BLUE_SCALE                     = 0x0D1A
const GL_BLUE_BIAS                      = 0x0D1B
const GL_ALPHA_SCALE                    = 0x0D1C
const GL_ALPHA_BIAS                     = 0x0D1D
const GL_DEPTH_SCALE                    = 0x0D1E
const GL_DEPTH_BIAS                     = 0x0D1F
const GL_MAX_EVAL_ORDER                 = 0x0D30
const GL_MAX_LIGHTS                     = 0x0D31
const GL_MAX_CLIP_PLANES                = 0x0D32
const GL_MAX_TEXTURE_SIZE               = 0x0D33
const GL_MAX_PIXEL_MAP_TABLE            = 0x0D34
const GL_MAX_ATTRIB_STACK_DEPTH         = 0x0D35
const GL_MAX_MODELVIEW_STACK_DEPTH      = 0x0D36
const GL_MAX_NAME_STACK_DEPTH           = 0x0D37
const GL_MAX_PROJECTION_STACK_DEPTH     = 0x0D38
const GL_MAX_TEXTURE_STACK_DEPTH        = 0x0D39
const GL_MAX_VIEWPORT_DIMS              = 0x0D3A
const GL_MAX_CLIENT_ATTRIB_STACK_DEPTH  = 0x0D3B
const GL_SUBPIXEL_BITS                  = 0x0D50
const GL_INDEX_BITS                     = 0x0D51
const GL_RED_BITS                       = 0x0D52
const GL_GREEN_BITS                     = 0x0D53
const GL_BLUE_BITS                      = 0x0D54
const GL_ALPHA_BITS                     = 0x0D55
const GL_DEPTH_BITS                     = 0x0D56
const GL_STENCIL_BITS                   = 0x0D57
const GL_ACCUM_RED_BITS                 = 0x0D58
const GL_ACCUM_GREEN_BITS               = 0x0D59
const GL_ACCUM_BLUE_BITS                = 0x0D5A
const GL_ACCUM_ALPHA_BITS               = 0x0D5B
const GL_NAME_STACK_DEPTH               = 0x0D70
const GL_AUTO_NORMAL                    = 0x0D80
const GL_MAP1_COLOR_4                   = 0x0D90
const GL_MAP1_INDEX                     = 0x0D91
const GL_MAP1_NORMAL                    = 0x0D92
const GL_MAP1_TEXTURE_COORD_1           = 0x0D93
const GL_MAP1_TEXTURE_COORD_2           = 0x0D94
const GL_MAP1_TEXTURE_COORD_3           = 0x0D95
const GL_MAP1_TEXTURE_COORD_4           = 0x0D96
const GL_MAP1_VERTEX_3                  = 0x0D97
const GL_MAP1_VERTEX_4                  = 0x0D98
const GL_MAP2_COLOR_4                   = 0x0DB0
const GL_MAP2_INDEX                     = 0x0DB1
const GL_MAP2_NORMAL                    = 0x0DB2
const GL_MAP2_TEXTURE_COORD_1           = 0x0DB3
const GL_MAP2_TEXTURE_COORD_2           = 0x0DB4
const GL_MAP2_TEXTURE_COORD_3           = 0x0DB5
const GL_MAP2_TEXTURE_COORD_4           = 0x0DB6
const GL_MAP2_VERTEX_3                  = 0x0DB7
const GL_MAP2_VERTEX_4                  = 0x0DB8
const GL_MAP1_GRID_DOMAIN               = 0x0DD0
const GL_MAP1_GRID_SEGMENTS             = 0x0DD1
const GL_MAP2_GRID_DOMAIN               = 0x0DD2
const GL_MAP2_GRID_SEGMENTS             = 0x0DD3
const GL_TEXTURE_1D                     = 0x0DE0
const GL_TEXTURE_2D                     = 0x0DE1
const GL_FEEDBACK_BUFFER_POINTER        = 0x0DF0
const GL_FEEDBACK_BUFFER_SIZE           = 0x0DF1
const GL_FEEDBACK_BUFFER_TYPE           = 0x0DF2
const GL_SELECTION_BUFFER_POINTER       = 0x0DF3
const GL_SELECTION_BUFFER_SIZE          = 0x0DF4
const GL_TEXTURE_WIDTH                  = 0x1000
const GL_TEXTURE_HEIGHT                 = 0x1001
const GL_TEXTURE_INTERNAL_FORMAT        = 0x1003
const GL_TEXTURE_BORDER_COLOR           = 0x1004
const GL_TEXTURE_BORDER                 = 0x1005

/* HintMode */
const GL_DONT_CARE                      = 0x1100
const GL_FASTEST                        = 0x1101
const GL_NICEST                         = 0x1102

/* LightName */
const GL_LIGHT0                         = 0x4000
const GL_LIGHT1                         = 0x4001
const GL_LIGHT2                         = 0x4002
const GL_LIGHT3                         = 0x4003
const GL_LIGHT4                         = 0x4004
const GL_LIGHT5                         = 0x4005
const GL_LIGHT6                         = 0x4006
const GL_LIGHT7                         = 0x4007

/* LightParameter */
const GL_AMBIENT                        = 0x1200
const GL_DIFFUSE                        = 0x1201
const GL_SPECULAR                       = 0x1202
const GL_POSITION                       = 0x1203
const GL_SPOT_DIRECTION                 = 0x1204
const GL_SPOT_EXPONENT                  = 0x1205
const GL_SPOT_CUTOFF                    = 0x1206
const GL_CONSTANT_ATTENUATION           = 0x1207
const GL_LINEAR_ATTENUATION             = 0x1208
const GL_QUADRATIC_ATTENUATION          = 0x1209

/* ListMode */
const GL_COMPILE                        = 0x1300
const GL_COMPILE_AND_EXECUTE            = 0x1301

/* LogicOp */
const GL_CLEAR                          = 0x1500
const GL_AND                            = 0x1501
const GL_AND_REVERSE                    = 0x1502
const GL_COPY                           = 0x1503
const GL_AND_INVERTED                   = 0x1504
const GL_NOOP                           = 0x1505
const GL_XOR                            = 0x1506
const GL_OR                             = 0x1507
const GL_NOR                            = 0x1508
const GL_EQUIV                          = 0x1509
const GL_INVERT                         = 0x150A
const GL_OR_REVERSE                     = 0x150B
const GL_COPY_INVERTED                  = 0x150C
const GL_OR_INVERTED                    = 0x150D
const GL_NAND                           = 0x150E
const GL_SET                            = 0x150F

/* MaterialParameter */
const GL_EMISSION                       = 0x1600
const GL_SHININESS                      = 0x1601
const GL_AMBIENT_AND_DIFFUSE            = 0x1602
const GL_COLOR_INDEXES                  = 0x1603

/* MatrixMode */
const GL_MODELVIEW                      = 0x1700
const GL_PROJECTION                     = 0x1701
const GL_TEXTURE                        = 0x1702

/* PixelCopyType */
const GL_COLOR                          = 0x1800
const GL_DEPTH                          = 0x1801
const GL_STENCIL                        = 0x1802

/* PixelFormat */
const GL_COLOR_INDEX                    = 0x1900
const GL_STENCIL_INDEX                  = 0x1901
const GL_DEPTH_COMPONENT                = 0x1902
const GL_RED                            = 0x1903
const GL_GREEN                          = 0x1904
const GL_BLUE                           = 0x1905
const GL_ALPHA                          = 0x1906
const GL_RGB                            = 0x1907
const GL_RGBA                           = 0x1908
const GL_LUMINANCE                      = 0x1909
const GL_LUMINANCE_ALPHA                = 0x190A

/* PixelType */
const GL_BITMAP                         = 0x1A00

/* PolygonMode */
const GL_POINT                          = 0x1B00
const GL_LINE                           = 0x1B01
const GL_FILL                           = 0x1B02

/* RenderingMode */
const GL_RENDER                         = 0x1C00
const GL_FEEDBACK                       = 0x1C01
const GL_SELECT                         = 0x1C02

/* ShadingModel */
const GL_FLAT                           = 0x1D00
const GL_SMOOTH                         = 0x1D01

/* StencilOp */
const GL_KEEP                           = 0x1E00
const GL_REPLACE                        = 0x1E01
const GL_INCR                           = 0x1E02
const GL_DECR                           = 0x1E03

/* StringName */
const GL_VENDOR                         = 0x1F00
const GL_RENDERER                       = 0x1F01
const GL_VERSION                        = 0x1F02
const GL_EXTENSIONS                     = 0x1F03

/* TextureCoordName */
const GL_S                              = 0x2000
const GL_T                              = 0x2001
const GL_R                              = 0x2002
const GL_Q                              = 0x2003

/* TextureEnvMode */
const GL_MODULATE                       = 0x2100
const GL_DECAL                          = 0x2101

/* TextureEnvParameter */
const GL_TEXTURE_ENV_MODE               = 0x2200
const GL_TEXTURE_ENV_COLOR              = 0x2201

/* TextureEnvTarget */
const GL_TEXTURE_ENV                    = 0x2300

/* TextureGenMode */
const GL_EYE_LINEAR                     = 0x2400
const GL_OBJECT_LINEAR                  = 0x2401
const GL_SPHERE_MAP                     = 0x2402

/* TextureGenParameter */
const GL_TEXTURE_GEN_MODE               = 0x2500
const GL_OBJECT_PLANE                   = 0x2501
const GL_EYE_PLANE                      = 0x2502

/* TextureMagFilter */
const GL_NEAREST                        = 0x2600
const GL_LINEAR                         = 0x2601

/* TextureMinFilter */
const GL_NEAREST_MIPMAP_NEAREST         = 0x2700
const GL_LINEAR_MIPMAP_NEAREST          = 0x2701
const GL_NEAREST_MIPMAP_LINEAR          = 0x2702
const GL_LINEAR_MIPMAP_LINEAR           = 0x2703

/* TextureParameterName */
const GL_TEXTURE_MAG_FILTER             = 0x2800
const GL_TEXTURE_MIN_FILTER             = 0x2801
const GL_TEXTURE_WRAP_S                 = 0x2802
const GL_TEXTURE_WRAP_T                 = 0x2803

/* TextureWrapMode */
const GL_CLAMP                          = 0x2900
const GL_REPEAT                         = 0x2901

/* ClientAttribMask */
const GL_CLIENT_PIXEL_STORE_BIT         = 0x00000001
const GL_CLIENT_VERTEX_ARRAY_BIT        = 0x00000002
const GL_CLIENT_ALL_ATTRIB_BITS         = 0xffffffff

/* polygon_offset */
const GL_POLYGON_OFFSET_FACTOR          = 0x8038
const GL_POLYGON_OFFSET_UNITS           = 0x2A00
const GL_POLYGON_OFFSET_POINT           = 0x2A01
const GL_POLYGON_OFFSET_LINE            = 0x2A02
const GL_POLYGON_OFFSET_FILL            = 0x8037

/* texture */
const GL_ALPHA4                         = 0x803B
const GL_ALPHA8                         = 0x803C
const GL_ALPHA12                        = 0x803D
const GL_ALPHA16                        = 0x803E
const GL_LUMINANCE4                     = 0x803F
const GL_LUMINANCE8                     = 0x8040
const GL_LUMINANCE12                    = 0x8041
const GL_LUMINANCE16                    = 0x8042
const GL_LUMINANCE4_ALPHA4              = 0x8043
const GL_LUMINANCE6_ALPHA2              = 0x8044
const GL_LUMINANCE8_ALPHA8              = 0x8045
const GL_LUMINANCE12_ALPHA4             = 0x8046
const GL_LUMINANCE12_ALPHA12            = 0x8047
const GL_LUMINANCE16_ALPHA16            = 0x8048
const GL_INTENSITY                      = 0x8049
const GL_INTENSITY4                     = 0x804A
const GL_INTENSITY8                     = 0x804B
const GL_INTENSITY12                    = 0x804C
const GL_INTENSITY16                    = 0x804D
const GL_R3_G3_B2                       = 0x2A10
const GL_RGB4                           = 0x804F
const GL_RGB5                           = 0x8050
const GL_RGB8                           = 0x8051
const GL_RGB10                          = 0x8052
const GL_RGB12                          = 0x8053
const GL_RGB16                          = 0x8054
const GL_RGBA2                          = 0x8055
const GL_RGBA4                          = 0x8056
const GL_RGB5_A1                        = 0x8057
const GL_RGBA8                          = 0x8058
const GL_RGB10_A2                       = 0x8059
const GL_RGBA12                         = 0x805A
const GL_RGBA16                         = 0x805B
const GL_TEXTURE_RED_SIZE               = 0x805C
const GL_TEXTURE_GREEN_SIZE             = 0x805D
const GL_TEXTURE_BLUE_SIZE              = 0x805E
const GL_TEXTURE_ALPHA_SIZE             = 0x805F
const GL_TEXTURE_LUMINANCE_SIZE         = 0x8060
const GL_TEXTURE_INTENSITY_SIZE         = 0x8061
const GL_PROXY_TEXTURE_1D               = 0x8063
const GL_PROXY_TEXTURE_2D               = 0x8064

/* texture_object */
const GL_TEXTURE_PRIORITY               = 0x8066
const GL_TEXTURE_RESIDENT               = 0x8067
const GL_TEXTURE_BINDING_1D             = 0x8068
const GL_TEXTURE_BINDING_2D             = 0x8069

/* vertex_array */
const GL_VERTEX_ARRAY                   = 0x8074
const GL_NORMAL_ARRAY                   = 0x8075
const GL_COLOR_ARRAY                    = 0x8076
const GL_INDEX_ARRAY                    = 0x8077
const GL_TEXTURE_COORD_ARRAY            = 0x8078
const GL_EDGE_FLAG_ARRAY                = 0x8079
const GL_VERTEX_ARRAY_SIZE              = 0x807A
const GL_VERTEX_ARRAY_TYPE              = 0x807B
const GL_VERTEX_ARRAY_STRIDE            = 0x807C
const GL_NORMAL_ARRAY_TYPE              = 0x807E
const GL_NORMAL_ARRAY_STRIDE            = 0x807F
const GL_COLOR_ARRAY_SIZE               = 0x8081
const GL_COLOR_ARRAY_TYPE               = 0x8082
const GL_COLOR_ARRAY_STRIDE             = 0x8083
const GL_INDEX_ARRAY_TYPE               = 0x8085
const GL_INDEX_ARRAY_STRIDE             = 0x8086
const GL_TEXTURE_COORD_ARRAY_SIZE       = 0x8088
const GL_TEXTURE_COORD_ARRAY_TYPE       = 0x8089
const GL_TEXTURE_COORD_ARRAY_STRIDE     = 0x808A
const GL_EDGE_FLAG_ARRAY_STRIDE         = 0x808C
const GL_VERTEX_ARRAY_POINTER           = 0x808E
const GL_NORMAL_ARRAY_POINTER           = 0x808F
const GL_COLOR_ARRAY_POINTER            = 0x8090
const GL_INDEX_ARRAY_POINTER            = 0x8091
const GL_TEXTURE_COORD_ARRAY_POINTER    = 0x8092
const GL_EDGE_FLAG_ARRAY_POINTER        = 0x8093
const GL_V2F                            = 0x2A20
const GL_V3F                            = 0x2A21
const GL_C4UB_V2F                       = 0x2A22
const GL_C4UB_V3F                       = 0x2A23
const GL_C3F_V3F                        = 0x2A24
const GL_N3F_V3F                        = 0x2A25
const GL_C4F_N3F_V3F                    = 0x2A26
const GL_T2F_V3F                        = 0x2A27
const GL_T4F_V4F                        = 0x2A28
const GL_T2F_C4UB_V3F                   = 0x2A29
const GL_T2F_C3F_V3F                    = 0x2A2A
const GL_T2F_N3F_V3F                    = 0x2A2B
const GL_T2F_C4F_N3F_V3F                = 0x2A2C
const GL_T4F_C4F_N3F_V4F                = 0x2A2D

/* Extensions */
const GL_EXT_vertex_array               = 1
const GL_EXT_bgra                       = 1
const GL_EXT_paletted_texture           = 1
const GL_WIN_swap_hint                  = 1
const GL_WIN_draw_range_elements        = 1

/* EXT_vertex_array */
const GL_VERTEX_ARRAY_EXT               = 0x8074
const GL_NORMAL_ARRAY_EXT               = 0x8075
const GL_COLOR_ARRAY_EXT                = 0x8076
const GL_INDEX_ARRAY_EXT                = 0x8077
const GL_TEXTURE_COORD_ARRAY_EXT        = 0x8078
const GL_EDGE_FLAG_ARRAY_EXT            = 0x8079
const GL_VERTEX_ARRAY_SIZE_EXT          = 0x807A
const GL_VERTEX_ARRAY_TYPE_EXT          = 0x807B
const GL_VERTEX_ARRAY_STRIDE_EXT        = 0x807C
const GL_VERTEX_ARRAY_COUNT_EXT         = 0x807D
const GL_NORMAL_ARRAY_TYPE_EXT          = 0x807E
const GL_NORMAL_ARRAY_STRIDE_EXT        = 0x807F
const GL_NORMAL_ARRAY_COUNT_EXT         = 0x8080
const GL_COLOR_ARRAY_SIZE_EXT           = 0x8081
const GL_COLOR_ARRAY_TYPE_EXT           = 0x8082
const GL_COLOR_ARRAY_STRIDE_EXT         = 0x8083
const GL_COLOR_ARRAY_COUNT_EXT          = 0x8084
const GL_INDEX_ARRAY_TYPE_EXT           = 0x8085
const GL_INDEX_ARRAY_STRIDE_EXT         = 0x8086
const GL_INDEX_ARRAY_COUNT_EXT          = 0x8087
const GL_TEXTURE_COORD_ARRAY_SIZE_EXT   = 0x8088
const GL_TEXTURE_COORD_ARRAY_TYPE_EXT   = 0x8089
const GL_TEXTURE_COORD_ARRAY_STRIDE_EXT = 0x808A
const GL_TEXTURE_COORD_ARRAY_COUNT_EXT  = 0x808B
const GL_EDGE_FLAG_ARRAY_STRIDE_EXT     = 0x808C
const GL_EDGE_FLAG_ARRAY_COUNT_EXT      = 0x808D
const GL_VERTEX_ARRAY_POINTER_EXT       = 0x808E
const GL_NORMAL_ARRAY_POINTER_EXT       = 0x808F
const GL_COLOR_ARRAY_POINTER_EXT        = 0x8090
const GL_INDEX_ARRAY_POINTER_EXT        = 0x8091
const GL_TEXTURE_COORD_ARRAY_POINTER_EXT = 0x8092
const GL_EDGE_FLAG_ARRAY_POINTER_EXT    = 0x8093
const GL_DOUBLE_EXT                     = GL_DOUBLE

/* EXT_bgra */
const GL_BGR_EXT                        = 0x80E0
const GL_BGRA_EXT                       = 0x80E1

/* EXT_paletted_texture */
/* These must match the GL_COLOR_TABLE_*_SGI enumerants */
const GL_COLOR_TABLE_FORMAT_EXT         = 0x80D8
const GL_COLOR_TABLE_WIDTH_EXT          = 0x80D9
const GL_COLOR_TABLE_RED_SIZE_EXT       = 0x80DA
const GL_COLOR_TABLE_GREEN_SIZE_EXT     = 0x80DB
const GL_COLOR_TABLE_BLUE_SIZE_EXT      = 0x80DC
const GL_COLOR_TABLE_ALPHA_SIZE_EXT     = 0x80DD
const GL_COLOR_TABLE_LUMINANCE_SIZE_EXT = 0x80DE
const GL_COLOR_TABLE_INTENSITY_SIZE_EXT = 0x80DF

const GL_COLOR_INDEX1_EXT               = 0x80E2
const GL_COLOR_INDEX2_EXT               = 0x80E3
const GL_COLOR_INDEX4_EXT               = 0x80E4
const GL_COLOR_INDEX8_EXT               = 0x80E5
const GL_COLOR_INDEX12_EXT              = 0x80E6
const GL_COLOR_INDEX16_EXT              = 0x80E7

/* WIN_draw_range_elements */
const GL_MAX_ELEMENTS_VERTICES_WIN      = 0x80E8
const GL_MAX_ELEMENTS_INDICES_WIN       = 0x80E9

/* WIN_phong_shading */
const GL_PHONG_WIN                      = 0x80EA
const GL_PHONG_HINT_WIN                 = 0x80EB

/* WIN_specular_fog */
const GL_FOG_SPECULAR_TEXTURE_WIN       = 0x80EC

/* For compatibility with OpenGL v1.0 */
const GL_LOGIC_OP = GL_INDEX_LOGIC_OP
const GL_TEXTURE_COMPONENTS = GL_TEXTURE_INTERNAL_FORMAT

#[swag.foreign("opengl32")]
{
    func glAccum(op: GLenum, value: GLfloat);
    func glAlphaFunc(fc: GLenum, ref: GLclampf);
    func glAreTexturesResident(n: GLsizei, textures: const *GLuint, residences: *GLboolean)->GLboolean;
    func glArrayElement(i: GLint);
    func glBegin(mode: GLenum);
    func glBindTexture(target: GLenum, texture: GLuint);
    func glBitmap(width: GLsizei, height: GLsizei, xorig: GLfloat, yorig: GLfloat, xmove: GLfloat, ymove: GLfloat, bitmap: const *GLubyte);
    func glBlendFunc(sfactor: GLenum, dfactor: GLenum);
    func glCallList(list: GLuint);
    func glCallLists(n: GLsizei, type: GLenum, lists: const *GLvoid);
    func glClear(mask: GLbitfield);
    func glClearAccum(red: GLfloat, green: GLfloat, blue: GLfloat, alpha: GLfloat);
    func glClearColor(red: GLclampf, green: GLclampf, blue: GLclampf, alpha: GLclampf);
    func glClearDepth(depth: GLclampd);
    func glClearIndex(c: GLfloat);
    func glClearStencil(s: GLint);
    func glClipPlane(plane: GLenum, equation: const *GLdouble);
    func glColor3b(red: GLbyte, green: GLbyte, blue: GLbyte);
    func glColor3bv(v: const *GLbyte);
    func glColor3d(red: GLdouble, green: GLdouble, blue: GLdouble);
    func glColor3dv(v: const *GLdouble);
    func glColor3f(red: GLfloat, green: GLfloat, blue: GLfloat);
    func glColor3fv(v: const *GLfloat);
    func glColor3i(red: GLint, green: GLint, blue: GLint);
    func glColor3iv(v: const *GLint);
    func glColor3s(red: GLshort, green: GLshort, blue: GLshort);
    func glColor3sv(v: const *GLshort);
    func glColor3ub(red: GLubyte, green: GLubyte, blue: GLubyte);
    func glColor3ubv(v: const GLubyte);
    func glColor3ui(red: GLuint, green: GLuint, blue: GLuint);
    func glColor3uiv(v: const *GLuint);
    func glColor3us(red: GLushort, green: GLushort, blue: GLushort);
    func glColor3usv(v: const *GLushort);
    func glColor4b(red: GLbyte, green: GLbyte, blue: GLbyte, alpha: GLbyte);
    func glColor4bv(v: const *GLbyte);
    func glColor4d(red: GLdouble, green: GLdouble, blue: GLdouble, alpha: GLdouble);
    func glColor4dv(v: const *GLdouble);
    func glColor4f(red: GLfloat, green: GLfloat, blue: GLfloat, alpha: GLfloat);
    func glColor4fv(v: const *GLfloat);
    func glColor4i(red: GLint, green: GLint, blue: GLint, alpha: GLint);
    func glColor4iv(v: const *GLint);
    func glColor4s(red: GLshort, green: GLshort, blue: GLshort, alpha: GLshort);
    func glColor4sv(v: const *GLshort);
    func glColor4ub(red: GLubyte, green: GLubyte, blue: GLubyte, alpha: GLubyte);
    func glColor4ubv(v: const GLubyte);
    func glColor4ui(red: GLuint, green: GLuint, blue: GLuint, alpha: GLuint);
    func glColor4uiv(v: const *GLuint);
    func glColor4us(red: GLushort, green: GLushort, blue: GLushort, alpha: GLushort);
    func glColor4usv(v: const GLushort);
    func glColorMask(red: GLboolean, green: GLboolean, blue: GLboolean, alpha: GLboolean);
    func glColorMaterial(face: GLenum, mode: GLenum);
    func glColorPointer(size: GLint, type: GLenum, stride: GLsizei, pointer: const *GLvoid);
    func glCopyPixels(x: GLint, y: GLint, width: GLsizei, height: GLsizei, type: GLenum);
    func glCopyTexImage1D(target: GLenum, level: GLint, internalFormat: GLenum, x: GLint, y: GLint, width: GLsizei, border: GLint);
    func glCopyTexImage2D(target: GLenum, level: GLint, internalFormat: GLenum, x: GLint, y: GLint, width: GLsizei, height: GLsizei, border: GLint);
    func glCopyTexSubImage1D(target: GLenum, level: GLint, xoffset: GLint, x: GLint, y: GLint, width: GLsizei);
    func glCopyTexSubImage2D(target: GLenum, level: GLint, xoffset: GLint, yoffset: GLint, x: GLint, y: GLint, width: GLsizei, height: GLsizei);
    func glCullFace(mode: GLenum);
    func glDeleteLists(list: GLuint, range: GLsizei);
    func glDeleteTextures(n: GLsizei, textures: const *GLuint);
    func glDepthFunc(fc: GLenum);
    func glDepthMask(flag: GLboolean);
    func glDepthRange(zNear: GLclampd, zFar: GLclampd);
    func glDisable(cap: GLenum);
    func glDisableClientState(array: GLenum);
    func glDrawArrays(mode: GLenum, first: GLint, count: GLsizei);
    func glDrawBuffer(mode: GLenum);
    func glDrawElements(mode: GLenum, count: GLsizei, type: GLenum, indices: const *GLvoid);
    func glDrawPixels(width: GLsizei, height: GLsizei, format: GLenum, type: GLenum, pixels: const *GLvoid);
    func glEdgeFlag(flag: GLboolean);
    func glEdgeFlagPointer(stride: GLsizei, pointer: const *GLvoid);
    func glEdgeFlagv(flag: const *GLboolean);
    func glEnable(cap: GLenum);
    func glEnableClientState(array: GLenum);
    func glEnd();
    func glEndList();
    func glEvalCoord1d(u: GLdouble);
    func glEvalCoord1dv(u: const *GLdouble);
    func glEvalCoord1f(u: GLfloat);
    func glEvalCoord1fv(u: const *GLfloat);
    func glEvalCoord2d(u: GLdouble, v: GLdouble);
    func glEvalCoord2dv(u: const *GLdouble);
    func glEvalCoord2f(u: GLfloat, v: GLfloat);
    func glEvalCoord2fv(u: const *GLfloat);
    func glEvalMesh1(mode: GLenum, i1: GLint, i2: GLint);
    func glEvalMesh2(mode: GLenum, i1: GLint, i2: GLint, j1: GLint, j2: GLint);
    func glEvalPoint1(i: GLint);
    func glEvalPoint2(i: GLint, j: GLint);
    func glFeedbackBuffer(size: GLsizei, type: GLenum, buffer: *GLfloat);
    func glFinish();
    func glFlush();
    func glFogf(pname: GLenum, param: GLfloat);
    func glFogfv(pname: GLenum, params: const *GLfloat);
    func glFogi(pname: GLenum, param: GLint);
    func glFogiv(pname: GLenum, params: const *GLint);
    func glFrontFace(mode: GLenum);
    func glFrustum(left: GLdouble, right: GLdouble, bottom: GLdouble, top: GLdouble, zNear: GLdouble, zFar: GLdouble);
    func glGenLists(range: GLsizei)->GLuint;
    func glGenTextures(n: GLsizei, textures: *GLuint);
    func glGetBooleanv(pname: GLenum, params: *GLboolean);
    func glGetClipPlane(plane: GLenum, equation: *GLdouble);
    func glGetDoublev(pname: GLenum, params: *GLdouble);
    func glGetError()->GLenum;
    func glGetFloatv(pname: GLenum, params: *GLfloat);
    func glGetIntegerv(pname: GLenum, params: *GLint);
    func glGetLightfv(light: GLenum, pname: GLenum, params: *GLfloat);
    func glGetLightiv(light: GLenum, pname: GLenum, params: *GLint);
    func glGetMapdv(target: GLenum, query: GLenum, v: *GLdouble);
    func glGetMapfv(target: GLenum, query: GLenum, v: *GLfloat);
    func glGetMapiv(target: GLenum, query: GLenum, v: *GLint);
    func glGetMaterialfv(face: GLenum, pname: GLenum, params: *GLfloat);
    func glGetMaterialiv(face: GLenum, pname: GLenum, params: *GLint);
    func glGetPixelMapfv(map: GLenum, values: *GLfloat);
    func glGetPixelMapuiv(map: GLenum, values: *GLuint);
    func glGetPixelMapusv(map: GLenum, values: *GLushort);
    func glGetPointerv(pname: GLenum, params: **GLvoid);
    func glGetPolygonStipple(mask: *GLubyte);
    func glGetString(name: GLenum)->const *GLubyte;
    func glGetTexEnvfv(target: GLenum, pname: GLenum, params: *GLfloat);
    func glGetTexEnviv(target: GLenum, pname: GLenum, params: *GLint);
    func glGetTexGendv(coord: GLenum, pname: GLenum, params: *GLdouble);
    func glGetTexGenfv(coord: GLenum, pname: GLenum, params: *GLfloat);
    func glGetTexGeniv(coord: GLenum, pname: GLenum, params: *GLint);
    func glGetTexImage(target: GLenum, level: GLint, format: GLenum, type: GLenum, pixels: *GLvoid);
    func glGetTexLevelParameterfv(target: GLenum, level: GLint, pname: GLenum, params: *GLfloat);
    func glGetTexLevelParameteriv(target: GLenum, level: GLint, pname: GLenum, params: *GLint);
    func glGetTexParameterfv(target: GLenum, pname: GLenum, params: *GLfloat);
    func glGetTexParameteriv(target: GLenum, pname: GLenum, params: *GLint);
    func glHint(target: GLenum, mode: GLenum);
    func glIndexMask(mask: GLuint);
    func glIndexPointer(type: GLenum, stride: GLsizei, pointer: const *GLvoid);
    func glIndexd(c: GLdouble);
    func glIndexdv(c: const *GLdouble);
    func glIndexf(c: GLfloat);
    func glIndexfv(c: const *GLfloat);
    func glIndexi(c: GLint);
    func glIndexiv(c: const *GLint);
    func glIndexs(c: GLshort);
    func glIndexsv(c: const *GLshort);
    func glIndexub(c: GLubyte);
    func glIndexubv(c: const *GLubyte);
    func glInitNames();
    func glInterleavedArrays(format: GLenum, stride: GLsizei, pointer: const *GLvoid);
    func glIsEnabled(cap: GLenum)->GLboolean;
    func glIsList(list: GLuint)->GLboolean;
    func glIsTexture(texture: GLuint)->GLboolean;
    func glLightModelf(pname: GLenum, param: GLfloat);
    func glLightModelfv(pname: GLenum, params: const *GLfloat);
    func glLightModeli(pname: GLenum, param: GLint );
    func glLightModeliv(pname: GLenum, params: const *GLint);
    func glLightf(light: GLenum, pname: GLenum, param: GLfloat);
    func glLightfv(light: GLenum, pname: GLenum, params: const *GLfloat);
    func glLighti(light: GLenum, pname: GLenum, param: GLint );
    func glLightiv(light: GLenum, pname: GLenum, params: const *GLint);
    func glLineStipple(factor: GLint, pattern: GLushort);
    func glLineWidth(width: GLfloat);
    func glListBase(base: GLuint);
    func glLoadIdentity();
    func glLoadMatrixd(m: const *GLdouble);
    func glLoadMatrixf(m: const *GLfloat);
    func glLoadName(name: GLuint);
    func glLogicOp(opcode: GLenum);
    func glMap1d(target: GLenum, u1: GLdouble, u2: GLdouble, stride: GLint, order: GLint, points: const *GLdouble);
    func glMap1f(target: GLenum, u1: GLfloat, u2: GLfloat, stride: GLint, order: GLint, points: const *GLfloat);
    func glMap2d(target: GLenum, u1: GLdouble, u2: GLdouble, ustride: GLint, uorder: GLint, v1: GLdouble, v2: GLdouble, vstride: GLint, vorder: GLint, points: const *GLdouble);
    func glMap2f(target: GLenum, u1: GLfloat, u2: GLfloat, ustride: GLint, uorder: GLint, v1: GLfloat, v2: GLfloat, vstride: GLint, vorder: GLint, points: const *GLfloat);
    func glMapGrid1d(un: GLint, u1: GLdouble, u2: GLdouble);
    func glMapGrid1f(un: GLint, u1: GLfloat, u2: GLfloat);
    func glMapGrid2d(un: GLint, u1: GLdouble, u2: GLdouble, vn: GLint, v1: GLdouble, v2: GLdouble);
    func glMapGrid2f(un: GLint, u1: GLfloat, u2: GLfloat, vn: GLint, v1: GLfloat, v2: GLfloat);
    func glMaterialf(face: GLenum, pname: GLenum, param: GLfloat);
    func glMaterialfv(face: GLenum, pname: GLenum, params: const *GLfloat);
    func glMateriali(face: GLenum, pname: GLenum, param: GLint );
    func glMaterialiv(face: GLenum, pname: GLenum, params: const *GLint);
    func glMatrixMode(mode: GLenum);
    func glMultMatrixd(m: const *GLdouble);
    func glMultMatrixf(m: const *GLfloat);
    func glNewList(list: GLuint, mode: GLenum);
    func glNormal3b(nc: GLbyte, ny: GLbyte, nz: GLbyte);
    func glNormal3bv(v: const *GLbyte);
    func glNormal3d(nx: GLdouble, ny: GLdouble, nz: GLdouble);
    func glNormal3dv(v: const *GLdouble);
    func glNormal3f(nx: GLfloat, ny: GLfloat, nz: GLfloat);
    func glNormal3fv(v: const *GLfloat);
    func glNormal3i(nx: GLint, ny: GLint, nz: GLint);
    func glNormal3iv(v: const *GLint);
    func glNormal3s(nx: GLshort, ny: GLshort, nz: GLshort);
    func glNormal3sv(v: const *GLshort);
    func glNormalPointer(type: GLenum, stride: GLsizei, pointer: const *GLvoid);
    func glOrtho(left: GLdouble, right: GLdouble, bottom: GLdouble, top: GLdouble, zNear: GLdouble, zFar: GLdouble);
    func glPassThrough(token: GLfloat);
    func glPixelMapfv(map: GLenum, mapsize: GLsizei, values: const *GLfloat);
    func glPixelMapuiv(map: GLenum, mapsize: GLsizei, values: const *GLuint);
    func glPixelMapusv(map: GLenum, mapsize: GLsizei, values: const *GLushort);
    func glPixelStoref(pname: GLenum, param: GLfloat);
    func glPixelStorei(pname: GLenum, param: GLint );
    func glPixelTransferf(pname: GLenum, param: GLfloat);
    func glPixelTransferi(pname: GLenum, param: GLint );
    func glPixelZoom(xfactor: GLfloat, yfactor: GLfloat);
    func glPointSize(size: GLfloat);
    func glPolygonMode(face: GLenum, mode: GLenum);
    func glPolygonOffset(factor: GLfloat, units: GLfloat);
    func glPolygonStipple(mask: const *GLubyte);
    func glPopAttrib();
    func glPopClientAttrib();
    func glPopMatrix();
    func glPopName();
    func glPrioritizeTextures(n: GLsizei, textures: const *GLuint, priorities: const *GLclampf);
    func glPushAttrib(mask: GLbitfield);
    func glPushClientAttrib(mask: GLbitfield);
    func glPushMatrix();
    func glPushName(name: GLuint);
    func glRasterPos2d(x: GLdouble, y: GLdouble);
    func glRasterPos2dv(v: const *GLdouble);
    func glRasterPos2f(x: GLfloat, y: GLfloat);
    func glRasterPos2fv(v: const *GLfloat);
    func glRasterPos2i(x: GLint, y: GLint);
    func glRasterPos2iv(v: const *GLint);
    func glRasterPos2s(x: GLshort, y: GLshort);
    func glRasterPos2sv(v: const *GLshort);
    func glRasterPos3d(x: GLdouble, y: GLdouble, z: GLdouble);
    func glRasterPos3dv(v: const *GLdouble);
    func glRasterPos3f(x: GLfloat, y: GLfloat, z: GLfloat);
    func glRasterPos3fv(v: const *GLfloat);
    func glRasterPos3i(x: GLint, y: GLint, z: GLint);
    func glRasterPos3iv(v: const *GLint);
    func glRasterPos3s(x: GLshort, y: GLshort, z: GLshort);
    func glRasterPos3sv(v: const *GLshort);
    func glRasterPos4d(x: GLdouble, y: GLdouble, z: GLdouble, w: GLdouble);
    func glRasterPos4dv(v: const *GLdouble);
    func glRasterPos4f(x: GLfloat, y: GLfloat, z: GLfloat, w: GLfloat);
    func glRasterPos4fv(v: const *GLfloat);
    func glRasterPos4i(x: GLint, y: GLint, z: GLint, w: GLint);
    func glRasterPos4iv(v: const *GLint);
    func glRasterPos4s(x: GLshort, y: GLshort, z: GLshort, w: GLshort);
    func glRasterPos4sv(v: const *GLshort);
    func glReadBuffer(mode: GLenum);
    func glReadPixels(x: GLint, y: GLint, width: GLsizei, height: GLsizei, format: GLenum, type: GLenum, pixels: *GLvoid);
    func glRectd(x1: GLdouble, y1: GLdouble, x2: GLdouble, y2: GLdouble);
    func glRectdv(v1: const *GLdouble, v2: const *GLdouble);
    func glRectf(x1: GLfloat, y1: GLfloat, x2: GLfloat, y2: GLfloat);
    func glRectfv(v1: const *GLfloat, v2: const *GLfloat);
    func glRecti(x1: GLint, y1: GLint, x2: GLint, y2: GLint);
    func glRectiv(v1: const *GLint, v2: const *GLint);
    func glRects(x1: GLshort, y1: GLshort, x2: GLshort, y2: GLshort);
    func glRectsv(v1: const *GLshort, v2: const *GLshort);
    func glRenderMode(mode: GLenum)->GLint;
    func glRotated(angle: GLdouble, x: GLdouble, y: GLdouble, z: GLdouble);
    func glRotatef(angle: GLfloat, x: GLfloat, y: GLfloat, z: GLfloat);
    func glScaled(x: GLdouble, y: GLdouble, z: GLdouble);
    func glScalef(x: GLfloat, y: GLfloat, z: GLfloat);
    func glScissor(x: GLint, y: GLint, width: GLsizei, height: GLsizei);
    func glSelectBuffer(size: GLsizei, buffer: *GLuint);
    func glShadeModel(mode: GLenum);
    func glStencilFunc(fc: GLenum , ref: GLint, mask: GLuint);
    func glStencilMask(mask: GLuint);
    func glStencilOp(fail: GLenum, zfail: GLenum, zpass: GLenum);
    func glTexCoord1d(s: GLdouble);
    func glTexCoord1dv(v: const *GLdouble);
    func glTexCoord1f(s: GLfloat);
    func glTexCoord1fv(v: const *GLfloat);
    func glTexCoord1i(s: GLint);
    func glTexCoord1iv(v: const *GLint);
    func glTexCoord1s(s: GLshort);
    func glTexCoord1sv(v: const *GLshort);
    func glTexCoord2d(s: GLdouble, t: GLdouble);
    func glTexCoord2dv(v: const *GLdouble);
    func glTexCoord2f(s: GLfloat, t: GLfloat);
    func glTexCoord2fv(v: const *GLfloat);
    func glTexCoord2i(s: GLint, t: GLint);
    func glTexCoord2iv(v: const *GLint);
    func glTexCoord2s(s: GLshort, t: GLshort);
    func glTexCoord2sv(v: const *GLshort);
    func glTexCoord3d(s: GLdouble, t: GLdouble, r: GLdouble);
    func glTexCoord3dv(v: const *GLdouble);
    func glTexCoord3f(s: GLfloat, t: GLfloat, r: GLfloat);
    func glTexCoord3fv(v: const *GLfloat);
    func glTexCoord3i(s: GLint, t: GLint, r: GLint);
    func glTexCoord3iv(v: const *GLint);
    func glTexCoord3s(s: GLshort, t: GLshort, r: GLshort);
    func glTexCoord3sv(v: const *GLshort);
    func glTexCoord4d(s: GLdouble, t: GLdouble, r: GLdouble, q: GLdouble);
    func glTexCoord4dv(v: const *GLdouble);
    func glTexCoord4f(s: GLfloat, t: GLfloat, r: GLfloat, q: GLfloat);
    func glTexCoord4fv(v: const *GLfloat);
    func glTexCoord4i(s: GLint, t: GLint, r: GLint, q: GLint);
    func glTexCoord4iv(v: const *GLint);
    func glTexCoord4s(s: GLshort, t: GLshort, r: GLshort, q: GLshort);
    func glTexCoord4sv(v: const *GLshort);
    func glTexCoordPointer(size: GLint, type: GLenum, stride: GLsizei, pointer: const *GLvoid);
    func glTexEnvf(target: GLenum, pname: GLenum, param: GLfloat);
    func glTexEnvfv(target: GLenum, pname: GLenum, params: const *GLfloat);
    func glTexEnvi(target: GLenum, pname: GLenum, param: GLint);
    func glTexEnviv(target: GLenum, pname: GLenum, params: const *GLint);
    func glTexGend(coord: GLenum, pname: GLenum, param: GLdouble);
    func glTexGendv(coord: GLenum, pname: GLenum, params: const *GLdouble);
    func glTexGenf(coord: GLenum, pname: GLenum, param: GLfloat);
    func glTexGenfv(coord: GLenum, pname: GLenum, params: const *GLfloat);
    func glTexGeni(coord: GLenum, pname: GLenum, param: GLint);
    func glTexGeniv(coord: GLenum, pname: GLenum, params: const *GLint);
    func glTexImage1D(target: GLenum, level: GLint, internalformat: GLint, width: GLsizei, border: GLint, format: GLenum, type: GLenum, pixels: const *GLvoid);
    func glTexImage2D(target: GLenum, level: GLint, internalformat: GLint, width: GLsizei, height: GLsizei, border: GLint, format: GLenum, type: GLenum, pixels: const *GLvoid);
    func glTexParameterf(target: GLenum, pname: GLenum, param: GLfloat);
    func glTexParameterfv(target: GLenum, pname: GLenum, params: const  *GLfloat);
    func glTexParameteri(target: GLenum, pname: GLenum, param: GLint );
    func glTexParameteriv(target: GLenum, pname: GLenum, params: const  *GLint);
    func glTexSubImage1D(target: GLenum, level: GLint, xoffset: GLint, width: GLsizei, format: GLenum, type: GLenum, pixels: const *GLvoid);
    func glTexSubImage2D(target: GLenum, level: GLint, xoffset: GLint, yoffset: GLint, width: GLsizei, height: GLsizei, format: GLenum, type: GLenum, pixels: const *GLvoid);
    func glTranslated(x: GLdouble, y: GLdouble, z: GLdouble);
    func glTranslatef(x: GLfloat, y: GLfloat, z: GLfloat);
    func glVertex2d(x: GLdouble, y: GLdouble);
    func glVertex2dv(v: const *GLdouble);
    func glVertex2f(x: GLfloat, y: GLfloat);
    func glVertex2fv(v: const *GLfloat);
    func glVertex2i(x: GLint, y: GLint);
    func glVertex2iv(v: const *GLint);
    func glVertex2s(x: GLshort, y: GLshort);
    func glVertex2sv(v: const *GLshort);
    func glVertex3d(x: GLdouble, y: GLdouble, z: GLdouble);
    func glVertex3dv(v: const *GLdouble);
    func glVertex3f(x: GLfloat, y: GLfloat, z: GLfloat);
    func glVertex3fv(v: const *GLfloat);
    func glVertex3i(x: GLint, y: GLint, z: GLint);
    func glVertex3iv(v: const *GLint);
    func glVertex3s(x: GLshort, y: GLshort, z: GLshort);
    func glVertex3sv(v: const *GLshort);
    func glVertex4d(x: GLdouble, y: GLdouble, z: GLdouble, w: GLdouble);
    func glVertex4dv(v: const *GLdouble);
    func glVertex4f(x: GLfloat, y: GLfloat, z: GLfloat, w: GLfloat);
    func glVertex4fv(v: const *GLfloat);
    func glVertex4i(x: GLint, y: GLint, z: GLint, w: GLint);
    func glVertex4iv(v: const *GLint);
    func glVertex4s(x: GLshort, y: GLshort, z: GLshort, w: GLshort);
    func glVertex4sv(v: const *GLshort);
    func glVertexPointer(size: GLint, type: GLenum, stride: GLsizei, pointer: const *GLvoid);
    func glViewport(x: GLint, y: GLint, width: GLsizei, height: GLsizei);

    //func glGenVertexArrays(n: GLsizei, arrays: *GLuint);
    //func glBindVertexArray(array: GLuint);
}
