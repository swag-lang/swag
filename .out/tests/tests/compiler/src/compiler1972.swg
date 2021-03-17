//#[swag.printbc]
#test
{
    enum RGB { R; G; B; }
    x := RGB
    @assert(@typeof(x) == const *swag.TypeInfoEnum)
    y := RGB.B
    @assert(y == 2)
}