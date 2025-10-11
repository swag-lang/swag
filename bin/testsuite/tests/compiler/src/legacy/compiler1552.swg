#test
{
    var buffer: [*] u8
    let count:  u32 = 0
    let str:    string = @mkstring(buffer, cast(u64) count)
}
