#test
{
    let buffer: const [..] u8 = [65, 66, 67]
    let str:    string = @mkstring(@dataof(buffer), cast(u64) @countof(buffer))
    @assert(str == "ABC")
}
