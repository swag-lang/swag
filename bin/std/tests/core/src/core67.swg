using Core

struct BABA {}

//#[Swag.PrintBc]
#test
{
    var arr: Array'BABA

    var x: BABA
    arr.add(moveref &x)
    @assert(arr.count == 1)
}
