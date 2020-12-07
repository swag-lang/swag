#import "std"
using std

//#[swag.printbc]
#test
{
    var str: String = "A⻨C"
    @assert(str.indexOf("A"'char).index == 0)
    @assert(str.indexOf("⻨"'char).index == 1)
    @assert(str.indexOf("C"'char).index == 4)
    @assert(str.indexOf("D"'char).success == false)

    (idx, success) := str.indexOf("A"'char)
    @assert(idx == 0)
    @assert(success == true)
}