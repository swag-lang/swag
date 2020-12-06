#import "std"
using std

struct BABA
{
    val: s32
}

//#[swag.printbc]
#test
{
    var arr: Array'BABA
    @assert(@typeof(arr).name == "std.Array'(__std68.BABA)")

    var x: BABA
    x.val = 666

    arr.emplace(&x)

    @assert(arr[0].val == 666)
}