#import "std"
using std.collections

struct BABA
{
}

//#[swag.printbc]
#test
{
    var arr: Array'BABA
    //@print(@typeof(arr).name)
    @assert(@typeof(arr).name == "std.collections.Array'(BABA)")

    var x: BABA
    arr.emplace(&x)
    @assert(arr.count == 1)
}