#import "std"
using std

//#[swag.printbc]
#test
{
    var str: String = "  ABC  "
    str.trim()
    @assert(str == "ABC")
}