
using Core

//#[Swag.PrintBc]
#test
{
    x := ".5"
    y := try StrConv.parseF64(x)
    @assert(y.value == 0.5)
}