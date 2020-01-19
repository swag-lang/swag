#import "std"
using std.collections
using std.text

//#[swag.printbc]
#test
{
    #ast
    {
        var str: String
        str += "x := 0\n"
        str += "loop 10\n"
        str += "x += 1\n"
        str += "loop 10\n"
        str += "x += 1\n"        
        @print(cast(string) {str.buffer, str.length})
        return ""//cast(string) str
    }

    //@assert(x == 10)
}