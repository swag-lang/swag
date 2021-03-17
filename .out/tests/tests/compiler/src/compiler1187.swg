struct TOTO
{
    x: s32
    #[swag.offset("x")]
    y: s32
}

#test
{
    var pt: TOTO
    pt.x = 10
    @assert(pt.y == 10)
}