
using Core

//#[Swag.PrintBc]
#test
{
    let str = "A⺗B"
    let result = Utf8.decodeLastRune(str)
    @assert(result.item0 == `B`'rune)
    @assert(result.item1 == 1)

    @assert(Utf8.countRunes(str) == 3)
}