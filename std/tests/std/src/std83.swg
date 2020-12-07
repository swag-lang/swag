#import "std"
using std

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "toto", 1, 666)
}