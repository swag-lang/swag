#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    format(&buf, "titi%%")
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "titi%")
}