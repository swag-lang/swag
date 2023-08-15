
using Core

//#[Swag.PrintBc]
#test
{
    var str = "A⺗B"
    var result = Utf8.decodeLastRune(str)
    @assert(result.item0 == `B`'rune)
    @assert(result.item1 == 1)

    @assert(Utf8.countRunes(str) == 3)
}