//#[Swag.PrintBc]
#test
{
    let a = 0'u32
    @assert(#typeof(a) == #typeof(u32))
    let xx = a
    @assert(#typeof(xx) == #typeof(u32))

    let tt = 0'u32
    @assert(#typeof(tt) == #typeof(u32))
}
