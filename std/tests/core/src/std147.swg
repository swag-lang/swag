#import "core"
using core

//#[swag.printbc]
#test
{
    var x: String = "指toto指指指titi指tata"
    arr := x.split("指"'char)
    @assert(arr.count == 3)
    @assert(arr[0] == "toto")
    @assert(arr[1] == "titi")
    @assert(arr[2] == "tata")
}