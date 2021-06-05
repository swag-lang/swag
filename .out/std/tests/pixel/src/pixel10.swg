using Core, FreeType2

#test
{
    var x: FT_Library
    err := cast(s32) FT_Init_FreeType(&x)
    Console.printf("%\n", err)
}