
using core

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

    ptr := b.lastBucket.datas
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
    //std.diagnostics.Debug.assert(false, "my message")

    var b: ConcatBuffer
    b.setBucketSize(4)
    b.addBytes(@[1,2,3,4,5,6,7,8], false)

    @assert(b.firstBucket != b.lastBucket)

    ptr := b.firstBucket.datas
    @assert(ptr[0] == 1)
    @assert(ptr[1] == 2)
    @assert(ptr[2] == 3)
    @assert(ptr[3] == 4)

    ptr1 := b.lastBucket.datas
    @assert(ptr1[0] == 5)
    @assert(ptr1[1] == 6)
    @assert(ptr1[2] == 7)
    @assert(ptr1[3] == 8)
}