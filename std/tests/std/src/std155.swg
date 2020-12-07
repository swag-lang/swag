#import "std"
using std

//#[swag.printbc]
#test
{
    //res := "A假CD"
    var res: String = "XXABAB"
    result := res.indexOf("AB")
    @assert(result.success == true)
    @assert(result.index == 2)

    result = res.indexOf("AB", 3)
    @assert(result.success == true)
    @assert(result.index == 4)

    result = res.indexOf("C")
    @assert(result.success == false)

    result = res.indexOf("CCCCCCCCC")
    @assert(result.success == false)

    result = res.indexOf("XXABAB")
    @assert(result.success == true)
    @assert(result.index == 0)

    result = res.indexOf("BAB")
    @assert(result.success == true)
    @assert(result.index == 3)
}