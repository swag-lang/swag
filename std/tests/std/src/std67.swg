#import "std"
using std

struct BABA
{
}

//#[swag.printbc]
#test
{
    var arr: Array'BABA

    var x: BABA
    arr.emplace(&x)
    @assert(arr.count == 1)
}