
using core

//#[Swag.printbc]
#test
{
    str := "A⺗B"
    result := utf8.decodeLastRune(str)
    @assert(result.item0 == "B"'rune)
    @assert(result.item1 == 1)

    @assert(utf8.countRunes(str) == 3)
}