#import "std"
using std.collections

struct BABA
{
}

//#[swag.printbc]
#test
{
    var arr: Array'BABA
    @print(@typeof(arr).name)
    @assert(@typeof(arr).name == "Array'BABA")
}