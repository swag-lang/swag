#import "std"
using std

//#[swag.printbc]
#test
{
    var x: String = "    toto titi  tata   "
    arr := x.split(" "'char)
    @assert(arr.count == 3)
    @assert(arr[0] == "toto")
    @assert(arr[1] == "titi")
    @assert(arr[2] == "tata")
}