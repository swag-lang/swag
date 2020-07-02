#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    convertAny(&buf, 1)
}