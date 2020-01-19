#test
{
    var buffer: [..] u8 = [65, 66, 67]
    let str: string = {@dataof(buffer), @countof(buffer)}
    @assert(str == "ABC")
}