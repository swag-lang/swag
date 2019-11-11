#import "std"
using std

//#[swag.printbc]
#test
{
    var x: Utf8 = "coucou"
    @assert(x.length == 6)
    x = null
    @assert(x.length == 0)
}