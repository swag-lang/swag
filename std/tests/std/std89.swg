#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    format(&buf, "%", 3'u32)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "3")
}
