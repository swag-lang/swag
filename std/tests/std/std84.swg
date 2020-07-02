#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    @print("starts\n")
    convertAny(&buf, 1)
}