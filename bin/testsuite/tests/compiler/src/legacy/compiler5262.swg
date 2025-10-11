var G = 55

#[Swag.Inline]
func toto()->&s32
{
    return &G
}

#test
{
    let a = &toto()
    @assert(#typeof(a) == *s32)
    @assert(dref a == 55)
}
