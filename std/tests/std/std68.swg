#import "std"
using std.collections

struct BABA
{
    val: s32
}

//#[swag.printbc]
#test
{
    var arr: Array'BABA
    //@print(@typeof(arr).name)
    @assert(@typeof(arr).name == "Array'BABA")

    var x: BABA
    x.val = 666

    arr.emplaceBack(&x)

    @assert(arr[0].val == 666)
}