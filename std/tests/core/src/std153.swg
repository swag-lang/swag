#import "core"
using core

//#[swag.printbc]
#test
{
    res := "A假CD"
    @assert(utf8.byteIndex(res, 0).index == 0)
    @assert(utf8.byteIndex(res, 1).index == 1)
    @assert(utf8.byteIndex(res, 2).index == 4)
    @assert(utf8.byteIndex(res, 3).index == 5)
    @assert(utf8.byteIndex(res, 4).success == false)
    @assert(utf8.countChars(res) == 4)
}