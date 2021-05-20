
using Core

struct BABA
{
}

//#[Swag.printbc]
#test
{
    var arr: Array'BABA

    var x: BABA
    arr.add(x)
    @assert(arr.count == 1)
}