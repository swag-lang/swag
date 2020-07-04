#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    format(&buf, "% 汉 % %", 1, 2, 3)
    str := buf.toString()
    //@print(cast(string) str)
    @assert(str == "1 汉 2 3")
}
