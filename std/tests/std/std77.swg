#import "std"
using std.text

//#[swag.printbc]
#test
{
    var str: String = "A⻨C"
    @assert(str.indexOf("A"'char) == 0)
    @assert(str.indexOf("⻨"'char) == 1)
    @assert(str.indexOf("C"'char) == 4)
    @assert(str.indexOf("D"'char) == -1)
}