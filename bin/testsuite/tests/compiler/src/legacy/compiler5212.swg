//#[Swag.PrintBc]
#test
{
    let a: any = 42
    #assert(#typeof(a) == any)
    @assert(@kindof(a) == s32)

    let b = cast(s32) a
    @assert(b == 42)
    let c = cast(const &s32) a
    @assert(c == 42)
    @assert(#typeof(c) == #type const &s32)
}
