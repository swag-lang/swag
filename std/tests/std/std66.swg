#import "std"
using std.collections

struct BABA
{
}

//#[swag.printbc]
#test
{
    var arr: Array'BABA
    @assert(@typeof(arr).name == "Array'(BABA)")

    var x: BABA
    arr.addBack(x)
    @assert(arr.count == 1)
}