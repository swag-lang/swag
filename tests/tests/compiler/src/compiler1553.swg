#test
{
    var buffer: const [..] u8 = @[65, 66, 67]
    var str: string = cast(string) @mkslice(@dataof(buffer), cast(uint) @countof(buffer))
    @assert(str == "ABC")
}