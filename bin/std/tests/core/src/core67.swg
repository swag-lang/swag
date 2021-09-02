
using Core

struct BABA
{
}

//#[Swag.PrintBc]
#test
{
    var arr: Array'BABA

    var x: BABA
    arr.emplace(&x)
    @assert(arr.count == 1)
}