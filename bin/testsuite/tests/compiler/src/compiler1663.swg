struct TT
{
    value: s64 = 456
}

//#[Swag.PrintBc]
#run
{
    var addr:  [1] [*] [*] TT
    var addr1: [*] TT
    var value: TT

    addr[0] = cast([*] [*] TT) &addr1
    addr1   = cast([*] TT) &value

    //@print(addr1.value)
    @assert(addr[0, 0].value == 456)
}
