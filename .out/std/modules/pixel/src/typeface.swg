#global public
using Core, FreeType

protected var g_FT: FT_Library
#init
{
    catch FT_Init_FreeType(&g_FT)
}

struct TypeFace
{
    face: FT_Face
}

impl TypeFace
{
    // Load a new font
    func load(fullname: string, faceIndex: u32 = 0)->TypeFace throw
    {
        var result: retval
        fntRaw := try File.readAllBytes(fullname)
        try FT_New_Memory_Face(g_FT, fntRaw.buffer, cast(FT_Long) fntRaw.count, cast(FT_Long) faceIndex, &result.face)
        return result
    }
}