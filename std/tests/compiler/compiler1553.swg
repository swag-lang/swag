#test
{
    var buffer: const [..] u8 = @[65, 66, 67]
    let str: string = cast(string) @mkslice(@dataof(buffer), @countof(buffer))
    @assert(str == "ABC")
}