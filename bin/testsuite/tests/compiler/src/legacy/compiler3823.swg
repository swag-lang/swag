//#[Swag.PrintBc]
#test
{
    var a = 1
    var b = 2
    (a, b) = {b, a}
    @assert(a == 2)
    @assert(b == 1)
}
