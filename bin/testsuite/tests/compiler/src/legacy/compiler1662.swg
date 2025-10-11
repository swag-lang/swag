struct TT
{
    value: s64 = 456
}

//#[Swag.PrintBc]
#run
{
    var addr:  [1] [*] [*] s64
    var addr1: [*] s64
    var value: s64 = 666

    addr[0] = cast([*] [*] s64) &addr1
    addr1   = cast([*] s64) &value

    @assert(addr[0, 0, 0] == 666)
}
