using Core, FreeType

#test
{
    var x: FT_Library
    try FT_Init_FreeType(&x)

    var fntRaw = try File.readAllBytes(imagePath("arial.ttf"))

    var face: FT_Face
    try FT_New_Memory_Face(x, fntRaw.buffer, cast(FT_Long) fntRaw.count, 0, &face)
    //@assert(face.num_glyphs == 121)
    var ci1 = FT_Get_Char_Index(face, 'A''u32)
    var ci2 = FT_Get_Char_Index(face, 'Z''u32)
    //@assert(ci1 == 36)
    //@assert(ci2 == 61)

    try FT_Set_Pixel_Sizes(face, 0, 20)
    try FT_Load_Char(face, ci1, FT_LOAD_DEFAULT)
    try FT_Render_Glyph(face.glyph, FT_Render_Mode.FT_RENDER_MODE_NORMAL)
}
