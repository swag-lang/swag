#test
{
    var a: s64 = -1 <<% 31
    @assert(a == -2147483648)

    var b: s64 = (-1's64 <<% 31) - 1
    @assert(b == -2147483649)
}