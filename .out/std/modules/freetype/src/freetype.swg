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

const FT_MAX_MODULES = 32

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

#[Swag.foreign("freetype")]
{
    func FT_Init_FreeType(alibrary: *FT_Library)->FT_Error;
    func FT_Done_FreeType(alibrary: FT_Library)->FT_Error;
    func FT_New_Memory_Face(library: FT_Library, file_base: const *FT_Byte, file_size: FT_Long, face_index: FT_Long, aface: *FT_Face);
    func FT_Done_Face(face: FT_Face);
}