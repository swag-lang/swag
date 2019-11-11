#import "std"
using std

//#[swag.printbc]
#test
{
    var x: Utf8
    x.reserve(100)
    x = "toto"
    @assert(x.length == 4)
    x.reserve(0)
    x = "toto1"
    @assert(x.length == 5)    
}