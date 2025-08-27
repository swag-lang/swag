struct(T = s32) Toto
{
    x:   T
}

#test
{
    var pt: Toto's32
    @assert(#typeof(pt.x) == s32)
    var pt1: Toto'f32
    @assert(#typeof(pt1.x) == f32)
}
