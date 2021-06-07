#global public
using Core, FreeType

protected var g_FT: FT_Library
#init
{
    catch FT_Init_FreeType(&g_FT)
}

struct TypeFace
{
    buffer: Array'u8
    face:   FT_Face
}

impl TypeFace
{
    // Load a new font
    func load(fullname: string, faceIndex: u32 = 0)->TypeFace throw
    {
        var result: retval
        result.buffer = try File.readAllBytes(fullname)
        try FT_New_Memory_Face(g_FT, result.buffer.buffer, cast(FT_Long) result.buffer.count, cast(FT_Long) faceIndex, &result.face)
        return result
    }

    // Returns the glyph decriptor of a given rune
    func getGlyphOutline(using self, c: rune, pathList: *LinePathList, size: u32)
    {
        assume FT_Set_Pixel_Sizes(face, 0, size)
        assume FT_Load_Char(face, cast(FT_ULong) c, FT_LOAD_DEFAULT)
    }
}