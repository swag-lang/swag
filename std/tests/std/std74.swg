#import "std"
using std.text

//#[swag.printbc]
#test
{
    var str: String = "A⺗B   "
    str.trimRight()
    @assert(str == "A⺗B")
}