//#[Swag.PrintBc]
#test
{
    let x: any = 6
    let y  = x
    @assert(@kindof(x) == @kindof(y))
    @assert(#typeof(x) == #typeof(y))
    @assert(@dataof(x) == @dataof(y))
}
