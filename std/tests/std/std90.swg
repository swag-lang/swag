#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    format(&buf, "value: %", -3.1415)
    str := buf.toString()
    @print(cast(string) str)
    @assert(str == "value: -3.14150")
}
