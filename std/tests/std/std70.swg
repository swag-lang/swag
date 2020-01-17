#import "std"
using std.collections
using std.text

//#[swag.printbc]
#test
{
    var arr: Array'string
    arr.addBack("toto")
    arr.addBack("titi")
    @assert(@countof(arr) == 2)
    //@assert(arr.count == 2)
    //@assert(arr[0] == "toto")
    //@assert(arr[1] == "titi")
}