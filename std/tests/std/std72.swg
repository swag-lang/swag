#import "std"
using std.text

//#[swag.printbc]
#test
{
    var str: String = "  ⺗AA⺗"
    str.trimLeft()
    @assert(str == "⺗AA⺗")

    str.clear()
    loop 50
        str += "    "       
    str.trimLeft()
    @assert(str.length == 0)
    @assert(@countof(str) == 0)
    @assert(str == "")
}