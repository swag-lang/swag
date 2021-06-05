using Core, FreeType

#test
{
    var x: FT_Library
    err := cast(s32) FT_Init_FreeType(&x)

    fntRaw := try File.readAllBytes(imagePath("aAntiCorona.ttf"))

    var face: FT_Face
    FT_New_Memory_Face(x, fntRaw.buffer, cast(FT_Long) fntRaw.count, 0, &face)
    Console.printf("num glyphs: %\n", face.num_glyphs)
}