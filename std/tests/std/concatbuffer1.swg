#import "std"
using std.collections

#test
{
    var b: ConcatBuffer
    @assert(@countof(b) == 0)
    b.clear()
    b.free()
    @assert(@countof(b) == 0)
}

#test
{
    var b: ConcatBuffer
    b.addByte(55)
    @assert(@countof(b) == 1)
    b.addByte(66)
    @assert(@countof(b) == 2)

    ptr := b.curBucket + @sizeof(*void)
    @assert(ptr[0] == 55)
    @assert(ptr[1] == 66)

    b.clear()
    @assert(@countof(b) == 0)
    b.addByte(0)
    @assert(@countof(b) == 1)
    b.free()
    @assert(@countof(b) == 0)
}

#test
{
    var b: ConcatBuffer
    b.setBucketSize(50)
    loop 50
        b.addByte(0xFE)
}

#test
{
    var b: ConcatBuffer
    b.setBucketSize(4)
    b.addBytes([1,2,3,4,5,6,7,8])

    ptr := b.firstBucket + @sizeof(*void)
    @assert(ptr[0] == 1)
    @assert(ptr[1] == 2)
    @assert(ptr[2] == 3)
    @assert(ptr[3] == 4)
}