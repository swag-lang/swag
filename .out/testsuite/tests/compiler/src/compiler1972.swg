//#[Swag.printbc]
#test
{
    enum RGB { R; G; B; }
    x := RGB
    @assert(@typeof(x) == const *Swag.TypeInfoEnum)
    y := RGB.B
    @assert(y == 2)
}