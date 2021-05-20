
using Core

//#[Swag.printbc]
#test
{
    res := "A假CD"
    @assert(Utf8.byteIndex(res, 0).index == 0)
    @assert(Utf8.byteIndex(res, 1).index == 1)
    @assert(Utf8.byteIndex(res, 2).index == 4)
    @assert(Utf8.byteIndex(res, 3).index == 5)
    @assert(Utf8.byteIndex(res, 4).success == false)
    @assert(Utf8.countRunes(res) == 4)
}