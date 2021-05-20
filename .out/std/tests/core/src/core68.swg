
using Core

struct BABA
{
    val: s32
}

//#[Swag.printbc]
#test
{
    var arr: Array'BABA
    @assert(@typeof(arr).name == "Core.Array'(__core68.BABA)")

    var x: BABA
    x.val = 666

    arr.emplace(&x)

    @assert(arr[0].val == 666)
}