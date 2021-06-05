#test
{
    var buffer: *u8
    var count: u32 = 0
    var str: string = @mkstring(buffer, cast(uint) count)
}