//#[Swag.PrintBc]
#test
{
    var a: any = 42
    #assert(#typeof(a) == any)
    @assert(@kindof(a) == s32)

    let b = cast(s32) a
    @assert(b == 42)
    let c = cast(const &s32) a
    @assert(#typeof(c) == #type const &s32)
    @assert(c == 42)

    a = true
}
