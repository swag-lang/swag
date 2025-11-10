//#[Swag.PrintBc]
#test
{
    let x = 6
    let y = cast(#decltype(x)) 6.0
    @assert(#typeof(x) == #typeof(y))
}
