
using core

struct BABA
{
}

//#[Swag.printbc]
#test
{
    var arr: Array'BABA

    var x: BABA
    arr.emplace(&x)
    @assert(arr.count == 1)
}