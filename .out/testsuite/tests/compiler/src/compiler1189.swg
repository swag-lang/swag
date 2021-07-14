struct TOTO
{
    b, g, r, a: u8
    #[Swag.Offset("b")]
    argb: u32
}

#test
{
    var pt: TOTO
    pt.a = 0xFF;
    pt.r = 0xFF;
    @assert(pt.argb == 0xFFFF0000)
    @assert(@sizeof(TOTO) == 4)
}