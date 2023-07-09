using Core

//#[Swag.PrintBc]
#test
{
    var c: ConcatBuffer
    c.setBucketSize(4)
    c.addNative(1'u32)
    var seek = c.getSeek()
    c.addNative(2'u32)
    c.setSeek(seek)
    c.addNative(3'u64)
}