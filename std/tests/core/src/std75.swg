#import "core"
using core

//#[swag.printbc]
#test
{
    var str: String = "ABC"
    visit(Bytes) c, i: str
    {
        switch i
        {
            case 0:
                @assert(c == cast(u8) "A"'char)
            case 1:
                @assert(c == cast(u8) "B"'char)
            case 2:
                @assert(c == cast(u8) "C"'char)
            default:
                @assert(false)
        }
    }
}