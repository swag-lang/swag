struct TT
{
    value:  s64 = 456
}

//#[swag.printbc]
#run
{
    var addr:  [1] **TT
    var addr1: *TT
    var value: TT

    addr[0]  = &addr1
    addr1 = &value

    //@print(addr1.value)
    @assert(addr[0,0].value == 456)
}