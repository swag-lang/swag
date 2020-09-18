#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "toto", 1, 666)
}