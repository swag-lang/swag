#import "std"
using std.text

//#[swag.printbc]
#test
{
    var str: String = "A⻨"
    visit c, i: str
    {
        switch i
        {
            case 0:
                @assert(c == "A"'char)
            case 1:
                @assert(c == "⻨"'char)
            default:
                @assert(false)
        }
    }
}