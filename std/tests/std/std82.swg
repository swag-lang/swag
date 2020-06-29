#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    convertInt(&buf, 123's32)
}
