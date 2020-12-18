#import "core"
using core

#test
{
    var buf: ConcatBuffer
    format.append(&buf, "toto", 1, 666)
}