using Core

struct BABA
{
    val: s32
}

//#[Swag.PrintBc]
#test
{
    var arr: Array'BABA
    @assert(@typeof(arr).fullname == "Core.Array'(__core68.BABA)")

    var x: BABA
    x.val = 666

    arr.add(moveref &x)

    @assert(arr[0].val == 666)
}
