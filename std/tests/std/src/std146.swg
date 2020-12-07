#import "std"
using std

//#[swag.printbc]
#test
{
    var x: String = "    toto titi  指事字   "
    arr := x.split(" "'char)
    @assert(arr.count == 3)
    @assert(arr[0] == "toto")
    @assert(arr[1] == "titi")
    @assert(arr[2] == "指事字")
}