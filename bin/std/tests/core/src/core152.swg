
using Core

//#[Swag.PrintBc]
#test
{
    var res: String = "A假C"
    var arr = res.toCharArray()
    @assert(arr.count == 3)
    @assert(arr[0] == `A`)
    @assert(arr[1] == `假`)
    @assert(arr[2] == `C`)
}