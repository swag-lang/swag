#import "std"
using std.text

//#[swag.printbc]
#test
{
    var str: String = "  ABC  "
    str.trim()
    @assert(str == "ABC")
}