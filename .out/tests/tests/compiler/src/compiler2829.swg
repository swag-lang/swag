#test
{
    var a: s64 = -1 <<% 31
    @assert(a == -2147483648)
}