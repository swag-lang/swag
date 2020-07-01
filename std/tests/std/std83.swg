#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    format(&buf, "toto", 1, 666)
}