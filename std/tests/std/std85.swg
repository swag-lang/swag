#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    format(&buf, "toto")
    str := buf.toString()
    @assert(str == "toto")
    //@print(cast(string) str)
}