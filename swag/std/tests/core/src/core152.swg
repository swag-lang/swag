
using core

//#[swag.printbc]
#test
{
    var res: String = "A假C"
    arr := res.toCharArray()
    @assert(arr.count == 3)
    @assert(arr[0] == "A"'char)
    @assert(arr[1] == "假"'char)
    @assert(arr[2] == "C"'char)
}