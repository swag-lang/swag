using Core, FreeType

#test
{
    var x: FT_Library
    try FT_Init_FreeType(&x)

    fntRaw := try File.readAllBytes(imagePath("aAntiCorona.ttf"))

    var face: FT_Face
    FT_New_Memory_Face(x, fntRaw.buffer, cast(FT_Long) fntRaw.count, 0, &face)
    @assert(face.num_glyphs == 121)
    ci1 := FT_Get_Char_Index(face, "A"'u32)
    ci2 := FT_Get_Char_Index(face, "Z"'u32)
    @assert(ci1 == 36)
    @assert(ci2 == 61)
}