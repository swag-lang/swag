#global export

alias FT_Library        = *FT_LibraryRec_
alias FT_Face           = *FT_FaceRec_
alias FT_CharMap        = *FT_CharMapRec_
alias FT_GlyphSlot      = *FT_GlyphSlotRec_
alias FT_SubGlyph       = *FT_SubGlyphRec_
alias FT_Size           = *FT_SizeRec_
alias FT_Slot_Internal  = *void
alias FT_Size_Internal  = *void
alias FT_Face_Internal  = *void
alias FT_Memory         = *void
alias FT_Module         = *void
alias FT_Renderer       = *void
alias FT_DebugHook_Func = *void
alias FT_ListNode       = *void
alias FT_Driver         = *void
alias FT_Stream         = *void

alias FT_String = u8
alias FT_Int    = s32
alias FT_UInt   = u32
alias FT_Long   = s32
alias FT_ULong  = u32
alias FT_Short  = s16
alias FT_UShort = u16
alias FT_Pos    = s32
alias FT_Error  = s32
alias FT_Fixed  = s32
alias FT_Byte   = u8
alias FT_Int32  = s32

const FT_LOAD_DEFAULT                       = 0x0
const FT_LOAD_NO_SCALE                      = 1 << 0
const FT_LOAD_NO_HINTING                    = 1 << 1
const FT_LOAD_RENDER                        = 1 << 2
const FT_LOAD_NO_BITMAP                     = 1 << 3
const FT_LOAD_VERTICAL_LAYOUT               = 1 << 4
const FT_LOAD_FORCE_AUTOHINT                = 1 << 5
const FT_LOAD_CROP_BITMAP                   = 1 << 6
const FT_LOAD_PEDANTIC                      = 1 << 7
const FT_LOAD_ADVANCE_ONLY                  = 1 << 8
const FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH   = 1 << 9
const FT_LOAD_NO_RECURSE                    = 1 << 10
const FT_LOAD_IGNORE_TRANSFORM              = 1 << 11
const FT_LOAD_MONOCHROME                    = 1 << 12
const FT_LOAD_LINEAR_DESIGN                 = 1 << 13
const FT_LOAD_SBITS_ONLY                    = 1 << 14
const FT_LOAD_NO_AUTOHINT                   = 1 << 15
const FT_LOAD_COLOR                         = 1 << 20
const FT_LOAD_COMPUTE_METRICS               = 1 << 21
const FT_LOAD_BITMAP_METRICS_ONLY           = 1 << 22

const FT_MAX_MODULES = 32

enum FT_Render_Mode
{
    FT_RENDER_MODE_NORMAL = 0
    FT_RENDER_MODE_LIGHT
    FT_RENDER_MODE_MONO
    FT_RENDER_MODE_LCD
    FT_RENDER_MODE_LCD_V
    FT_RENDER_MODE_MAX
}

struct FT_LibraryRec_
{
    memory:             FT_Memory
    version_major:      FT_Int
    version_minor:      FT_Int
    version_patch:      FT_Int
    num_modules:        FT_UInt
    modules:            [FT_MAX_MODULES] FT_Module
    renderers:          FT_ListRec
    cur_renderer:       FT_Renderer
    auto_hinter:        FT_Module
    debug_hooks:        [4] FT_DebugHook_Func
    lcd_geometry:       [3] FT_Vector
    refcount:           FT_Int
}

struct FT_FaceRec_
{
    num_faces:              FT_Long
    face_index:             FT_Long
    face_flags:             FT_Long
    style_flags:            FT_Long
    num_glyphs:             FT_Long
    family_name:            *FT_String
    style_name:             *FT_String
    num_fixed_sizes:        FT_Int
    available_sizes:        *FT_Bitmap_Size
    num_charmaps:           FT_Int
    charmaps:               *FT_CharMap
    generic:                FT_Generic
    bbox:                   FT_BBox
    units_per_EM:           FT_UShort
    ascender:               FT_Short
    descender:              FT_Short
    height:                 FT_Short
    max_advance_width:      FT_Short
    max_advance_height:     FT_Short
    underline_position:     FT_Short
    underline_thickness:    FT_Short
    glyph:                  FT_GlyphSlot
    size:                   FT_Size
    charmap:                FT_CharMap
    driver:                 FT_Driver
    memory:                 FT_Memory
    stream:                 FT_Stream
    sizes_list:             FT_ListRec
    autohint:               FT_Generic
    extensions:             *void
    internal:               FT_Face_Internal
}

struct FT_BBox
{
    xMin, yMin: FT_Pos
    xMax, yMax: FT_Pos
}

struct FT_Generic
{
    data:       *void
    finalizer:  *void // FT_Generic_Finalizer
}

struct FT_CharMapRec_
{
    face:           FT_Face
    encoding:       u32   // FT_Encoding
    platform_id:    FT_UShort
    encoding_id:    FT_UShort
}

struct FT_Bitmap_Size
{
    height: FT_Short
    width:  FT_Short
    size:   FT_Pos
    x_ppem: FT_Pos
    y_ppem: FT_Pos
}

struct FT_ListRec
{
    head: FT_ListNode
    tail: FT_ListNode
}

struct FT_Vector
{
    x:  FT_Pos
    y:  FT_Pos
}

struct FT_Matrix
{
    xx, xy: FT_Fixed
    yx, yy: FT_Fixed
}

struct FT_GlyphSlotRec_
{
    library:            FT_Library
    face:               FT_Face
    next:               FT_GlyphSlot
    glyph_index:        FT_UInt
    generic:            FT_Generic
    metrics:            FT_Glyph_Metrics
    linearHoriAdvance:  FT_Fixed
    linearVertAdvance:  FT_Fixed
    advance:            FT_Vector
    format:             FT_Glyph_Format = FT_GLYPH_FORMAT_NONE
    bitmap:             FT_Bitmap
    bitmap_left:        FT_Int
    bitmap_top:         FT_Int
    outline:            FT_Outline
    num_subglyphs:      FT_UInt
    subglyphs:          FT_SubGlyph
    control_data:       *void
    control_len:        s32
    lsb_delta:          FT_Pos
    rsb_delta:          FT_Pos
    other:              *void
    internal:           FT_Slot_Internal
}

#[Swag.constexpr]
func FT_IMAGE_TAG(x1, x2, x3, x4: u32)->u32 => (x1 << 24) | (x2 << 16) | (x3 << 8) | (x4)
enum FT_Glyph_Format
{
    FT_GLYPH_FORMAT_NONE      = 0
    FT_GLYPH_FORMAT_COMPOSITE = FT_IMAGE_TAG("c"'u32, "o"'u32, "m"'u32, "p"'u32)
    FT_GLYPH_FORMAT_BITMAP    = FT_IMAGE_TAG("b"'u32, "i"'u32, "t"'u32, "s"'u32)
    FT_GLYPH_FORMAT_OUTLINE   = FT_IMAGE_TAG("o"'u32, "u"'u32, "t"'u32, "l"'u32)
    FT_GLYPH_FORMAT_POLTTER   = FT_IMAGE_TAG("p"'u32, "l"'u32, "o"'u32, "t"'u32)
}

struct FT_Bitmap
{
    rows:           u32
    width:          u32
    pitch:          s32
    buffer:         *u8
    num_grays:      u16
    pixel_mode:     u8
    palette_mode:   u8
    palette:        *void
}

struct FT_Outline
{
    n_contours:     s16
    n_points:       s16
    points:         *FT_Vector
    tags:           *u8
    contours:       *s16
    flags:          s32
}

struct FT_Glyph_Metrics
{
    width:          FT_Pos
    height:         FT_Pos
    horiBearingX:   FT_Pos
    horiBearingY:   FT_Pos
    horiAdvance:    FT_Pos
    vertBearingX:   FT_Pos
    vertBearingY:   FT_Pos
    vertAdvance:    FT_Pos
}

struct FT_SubGlyphRec_
{
    index:      FT_Int
    flags:      FT_UShort
    arg1:       FT_Int
    arg2:       FT_Int
    transform:  FT_Matrix
}

struct FT_SizeRec_
{
    face:       FT_Face
    generic:    FT_Generic
    metrics:    FT_Size_Metrics
    internal:   FT_Size_Internal
}

struct FT_Size_Metrics
{
    x_ppem:         FT_UShort
    y_ppem:         FT_UShort
    x_scale:        FT_Fixed
    y_scale:        FT_Fixed
    ascender:       FT_Pos
    descender:      FT_Pos
    height:         FT_Pos
    max_advance:    FT_Pos
}

var Errors: [0xFF] string = ?
#run
{
    Errors[0x01] = "cannot open resource"
    Errors[0x02] = "unknown file format"
    Errors[0x03] = "broken file"
    Errors[0x04] = "invalid FreeType version"
    Errors[0x05] = "module version is too low"
    Errors[0x06] = "invalid argument"
    Errors[0x07] = "unimplemented feature"
    Errors[0x08] = "broken table"
    Errors[0x09] = "broken offset within table"
    Errors[0x0A] = "array allocation size too large"
    Errors[0x0B] = "missing module"
    Errors[0x0C] = "missing property"
    Errors[0x10] = "invalid glyph index"
    Errors[0x11] = "invalid character code"
    Errors[0x12] = "unsupported glyph image format"
    Errors[0x13] = "cannot render this glyph format"
    Errors[0x14] = "invalid outline"
    Errors[0x15] = "invalid composite glyph"
    Errors[0x16] = "too many hints"
    Errors[0x17] = "invalid pixel size"
    Errors[0x20] = "invalid object handle"
    Errors[0x21] = "invalid library handle"
    Errors[0x22] = "invalid module handle"
    Errors[0x23] = "invalid face handle"
    Errors[0x24] = "invalid size handle"
    Errors[0x25] = "invalid glyph slot handle"
    Errors[0x26] = "invalid charmap handle"
    Errors[0x27] = "invalid cache manager handle"
    Errors[0x28] = "invalid stream handle"
    Errors[0x30] = "too many modules"
    Errors[0x31] = "too many extensions"
    Errors[0x40] = "out of memory"
    Errors[0x41] = "unlisted object"
    Errors[0x51] = "cannot open stream"
    Errors[0x52] = "invalid stream seek"
    Errors[0x53] = "invalid stream skip"
    Errors[0x54] = "invalid stream read"
    Errors[0x55] = "invalid stream operation"
    Errors[0x56] = "invalid frame operation"
    Errors[0x57] = "nested frame access"
    Errors[0x58] = "invalid frame read"
    Errors[0x60] = "raster uninitialized"
    Errors[0x61] = "raster corrupted"
    Errors[0x62] = "raster overflow"
    Errors[0x63] = "negative height while rastering"
    Errors[0x70] = "too many registered caches"
    Errors[0x80] = "invalid opcode"
    Errors[0x81] = "too few arguments"
    Errors[0x82] = "stack overflow"
    Errors[0x83] = "code overflow"
    Errors[0x84] = "bad argument"
    Errors[0x85] = "division by zero"
    Errors[0x86] = "invalid reference"
    Errors[0x87] = "found debug opcode"
    Errors[0x88] = "found ENDF opcode in execution stream"
    Errors[0x89] = "nested DEFS"
    Errors[0x8A] = "invalid code range"
    Errors[0x8B] = "execution context too long"
    Errors[0x8C] = "too many function definitions"
    Errors[0x8D] = "too many instruction definitions"
    Errors[0x8E] = "SFNT font table missing"
    Errors[0x8F] = "horizontal header (hhea) table missing"
    Errors[0x90] = "locations (loca) table missing"
    Errors[0x91] = "name table missing"
    Errors[0x92] = "character map (cmap) table missing"
    Errors[0x93] = "horizontal metrics (hmtx) table missing"
    Errors[0x94] = "PostScript (post) table missing"
    Errors[0x95] = "invalid horizontal metrics"
    Errors[0x96] = "invalid character map (cmap) format"
    Errors[0x97] = "invalid ppem value"
    Errors[0x98] = "invalid vertical metrics"
    Errors[0x99] = "could not find context"
    Errors[0x9A] = "invalid PostScript (post) table format"
    Errors[0x9B] = "invalid PostScript (post) table"
    Errors[0x9C] = "found FDEF or IDEF opcode in glyf bytecode"
    Errors[0x9D] = "missing bitmap in strike"
    Errors[0xA0] = "opcode syntax error"
    Errors[0xA1] = "argument stack underflow"
    Errors[0xA2] = "ignore"
    Errors[0xA3] = "no Unicode glyph name found"
    Errors[0xA4] = "glyph too big for hinting"
    Errors[0xB0] = "`STARTFONT' field missing"
    Errors[0xB1] = "`FONT' field missing"
    Errors[0xB2] = "`SIZE' field missing"
    Errors[0xB3] = "`FONTBOUNDINGBOX' field missing"
    Errors[0xB4] = "`CHARS' field missing"
    Errors[0xB5] = "`STARTCHAR' field missing"
    Errors[0xB6] = "`ENCODING' field missing"
    Errors[0xB7] = "`BBX' field missing"
    Errors[0xB8] = "`BBX' too big"
    Errors[0xB9] = "Font header corrupted or missing fields"
    Errors[0xBA] = "Font glyphs corrupted or missing fields"
}

#[Swag.foreign("freetype")]
{
    func FT_New_Memory_Face(library: FT_Library, file_base: const *FT_Byte, file_size: FT_Long, face_index: FT_Long, aface: *FT_Face);
    func FT_Done_Face(face: FT_Face);
    func FT_Get_Char_Index(face: FT_Face, charcode: FT_ULong)->FT_UInt;
}

namespace FreeTypePriv
{
    #[Swag.foreign("freetype")]
    {
        func FT_Init_FreeType(alibrary: *FT_Library)->FT_Error;
        func FT_Done_FreeType(alibrary: FT_Library)->FT_Error;
        func FT_Set_Pixel_Sizes(face: FT_Face, pixel_width, pixel_height: FT_UInt)->FT_Error;
        func FT_Load_Glyph(face: FT_Face, glyph_index: FT_UInt, load_flags: FT_Int32)->FT_Error;
        func FT_Load_Char(face: FT_Face, char_code: FT_ULong, load_flags: FT_Int32)->FT_Error;
        func FT_Render_Glyph(slot: FT_GlyphSlot, render_mode: FT_Render_Mode)->FT_Error;
    }
}

#[Swag.inline]
{
    func FT_Init_FreeType(alibrary: *FT_Library) throw
    {
        err := FreeTypePriv.FT_Init_FreeType(alibrary)
        if err throw Errors[err]
    }

    func FT_Done_FreeType(alibrary: FT_Library) throw
    {
        err := FreeTypePriv.FT_Done_FreeType(alibrary)
        if err throw Errors[err]
    }

    func FT_Set_Pixel_Sizes(face: FT_Face, pixel_width, pixel_height: FT_UInt) throw
    {
        err := FreeTypePriv.FT_Set_Pixel_Sizes(face, pixel_width, pixel_height)
        if err throw Errors[err]
    }

    func FT_Load_Glyph(face: FT_Face, glyph_index: FT_UInt, load_flags: FT_Int32) throw
    {
        err := FreeTypePriv.FT_Load_Glyph(face, glyph_index, load_flags)
        if err throw Errors[err]
    }

    func FT_Load_Char(face: FT_Face, char_code: FT_ULong, load_flags: FT_Int32) throw
    {
        err := FreeTypePriv.FT_Load_Char(face, char_code, load_flags)
        if err throw Errors[err]
    }

    func FT_Render_Glyph(slot: FT_GlyphSlot, render_mode: FT_Render_Mode) throw
    {
        err := FreeTypePriv.FT_Render_Glyph(slot, render_mode)
        if err throw Errors[err]
    }
}