//#[Swag.PrintBc]
#test
{
    enum RGB { R, G, B }
    let x = RGB
    @assert(#typeof(x) == const *Swag.TypeInfoEnum)
    let y = RGB.B
    @assert(y == 2)
}
