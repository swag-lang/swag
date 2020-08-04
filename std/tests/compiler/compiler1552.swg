#test
{
    var buffer: *u8
    var count: u32 = 0
    let str: string = cast(string) @mkslice(buffer, count)
}