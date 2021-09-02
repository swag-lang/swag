
using Core

//#[Swag.PrintBc]
#test
{
    var res: String = "A假C"
    arr := res.toCharArray()
    @assert(arr.count == 3)
    @assert(arr[0] == "A"'rune)
    @assert(arr[1] == "假"'rune)
    @assert(arr[2] == "C"'rune)
}