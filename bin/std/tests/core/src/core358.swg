using Core

//#[Swag.PrintBc]
#test
{
    var str: String = "A"
    str[0] += 1
    @assert(str[0] == `B`'u8)
    str[0] -= 1
    @assert(str[0] == `A`'u8)
}
