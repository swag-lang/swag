#import "core"
using core

//#[swag.printbc]
#test
{
    str := "A⺗B"
    result := utf8.lastChar(str)
    @assert(result.item0 == "B"'char)
    @assert(result.item1 == 1)

    @assert(utf8.countChars(str) == 3)
}