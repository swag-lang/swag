
using Core, Core.Utf8

//#[Swag.PrintBc]
#test
{
    //var res = "A假CD"
    var res: String = "XXABAB"
    var result = indexOf(res.toString(), "AB")
    @assert(result == 2)

    result = indexOf(res.toString(), "AB", 3)
    @assert(result == 4)

    result = indexOf(res.toString(), "C")
    @assert(result == Swag.U64.Max)

    result = indexOf(res.toString(), "CCCCCCCCC")
    @assert(result == Swag.U64.Max)

    result = indexOf(res.toString(), "XXABAB")
    @assert(result == 0)

    result = indexOf(res.toString(), "BAB")
    @assert(result == 3)
}