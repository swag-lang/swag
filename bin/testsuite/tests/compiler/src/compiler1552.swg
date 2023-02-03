#test
{
    var buffer: ^u8
    var count: u32 = 0
    var str: string = @mkstring(buffer, cast(u64) count)
}