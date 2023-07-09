//#[Swag.PrintBc]
#test
{
    enum RGB { R; G; B; }
    var x = RGB
    @assert(@typeof(x) == const *Swag.TypeInfoEnum)
    var y = RGB.B
    @assert(y == 2)
}