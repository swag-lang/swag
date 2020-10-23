#import "std"
using std

//#[swag.printbc]
#test
{
    var x: String = "ABCDE昨"
    @assert(x.front() == "A"'char)
    @assert(x.back() == "昨"'char)
}