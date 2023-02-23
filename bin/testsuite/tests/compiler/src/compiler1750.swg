//#[Swag.PrintBc]
#test
{
    x := 6
    y := cast(@decltype(x)) 6.0
    @assert(@typeof(x) == @typeof(y))
}