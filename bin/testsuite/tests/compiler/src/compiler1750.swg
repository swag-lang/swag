//#[Swag.PrintBc]
#test
{
    var x = 6
    var y = cast(@decltype(x)) 6.0
    @assert(@typeof(x) == @typeof(y))
}