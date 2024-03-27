using Core

//#[Swag.PrintBc]
#test
{
    var c: ConcatBuffer
    c.setBucketSize(4)
    c.addNative(1'u32)
    c.addNative(2'u32)
    c.clear()
    c.addNative(1'u64)
}