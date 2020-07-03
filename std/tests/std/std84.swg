#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    @print("starts\n")
    convertAny(&buf, 666)
    str := buf.toString()
    @print(cast(string) str)
}