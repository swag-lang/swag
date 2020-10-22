#import "std"
using std

//#[swag.printbc]
#test
{
    var str: String = "A⺗B"
    var ptr = str.buffer + str.length

    result := utf8.previousChar(str.buffer, ptr)
    @assert(result.item1 == "B"'char)

    result = utf8.previousChar(str.buffer, result.item0)
    @assert(result.item1 == "⺗"'char)

    result = utf8.previousChar(str.buffer, result.item0)
    @assert(result.item1 == "A"'char)

    result = utf8.previousChar(str.buffer, result.item0)
    @assert(result.item0 == str.buffer)
    @assert(result.item1 == 0)

    @assert(utf8.countChars(str.buffer) == 3)
}