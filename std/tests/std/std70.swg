#import "std"
using std.collections

//#[swag.printbc]
#test
{
    var arr: Array'string
    arr.add("toto")
    arr.add("titi")
    @assert(@countof(arr) == 2)
    //@assert(arr.count == 2)
    //@assert(arr[0] == "toto")
    //@assert(arr[1] == "titi")
}