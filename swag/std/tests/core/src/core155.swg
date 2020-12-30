#import "core"
using core

//#[swag.printbc]
#test
{
    //res := "A假CD"
    var res: String = "XXABAB"
    result := res.indexOf("AB")
    @assert(result == 2)

    result = res.indexOf("AB", 3)
    @assert(result == 4)

    result = res.indexOf("C")
    @assert(result == swag.MaxUINT)

    result = res.indexOf("CCCCCCCCC")
    @assert(result == swag.MaxUINT)

    result = res.indexOf("XXABAB")
    @assert(result == 0)

    result = res.indexOf("BAB")
    @assert(result == 3)
}