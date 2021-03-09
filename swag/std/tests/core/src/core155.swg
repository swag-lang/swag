
using core, utf8

//#[swag.printbc]
#test
{
    //res := "A假CD"
    var res: String = "XXABAB"
    result := indexOf(res.toString(), "AB")
    @assert(result == 2)

    result = indexOf(res.toString(), "AB", 3)
    @assert(result == 4)

    result = indexOf(res.toString(), "C")
    @assert(result == swag.MaxUINT)

    result = indexOf(res.toString(), "CCCCCCCCC")
    @assert(result == swag.MaxUINT)

    result = indexOf(res.toString(), "XXABAB")
    @assert(result == 0)

    result = indexOf(res.toString(), "BAB")
    @assert(result == 3)
}