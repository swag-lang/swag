#import "std"
using std.text

//#[swag.printbc]
#test
{
    var str: String = "  ⺗AA⺗"
    str.trimLeft()
    @assert(str == "⺗AA⺗")
}