
using core

struct BABA
{
    val: s32
}

//#[Swag.printbc]
#test
{
    var arr: Array'BABA
    @assert(@typeof(arr).name == "core.Array'(__core68.BABA)")

    var x: BABA
    x.val = 666

    arr.emplace(&x)

    @assert(arr[0].val == 666)
}