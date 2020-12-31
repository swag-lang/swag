#import "core"
using core

//#[swag.printbc]
#test
{
    var str: String = "A⻨C"
    @assert(utf8.indexOf(str.toString(), "A"'char) == 0)
    @assert(utf8.indexOf(str.toString(), "⻨"'char) == 1)
    @assert(utf8.indexOf(str.toString(), "C"'char) == 4)
    @assert(utf8.indexOf(str.toString(), "D"'char) == swag.MaxUINT)

    idx := utf8.indexOf(str.toString(), "A"'char)
    @assert(idx == 0)
}