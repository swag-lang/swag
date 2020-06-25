#import "std"
using std.collections

#test
{
    var b: ConcatBuffer
    @assert(@countof(b) == 0)
}

#test
{
    var b: ConcatBuffer
    b.addByte(0)
    @assert(@countof(b) == 1)
    b.addByte(1)
    @assert(@countof(b) == 2)
    b.clear()
    @assert(@countof(b) == 0)
    b.addByte(0)
    @assert(@countof(b) == 1)
    b.free()
    @assert(@countof(b) == 0)
}