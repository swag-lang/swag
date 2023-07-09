
using Core

//#[Swag.PrintBc]
#test
{
    var x = ".5"
    var y = try StrConv.parseF64(x)
    @assert(y.value == 0.5)
}