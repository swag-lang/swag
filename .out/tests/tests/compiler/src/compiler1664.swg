struct TT
{
    value:  s64 = 456
}

//#[swag.printbc]
#run
{
    var addr:  [2, 2] *TT
    var value: TT

    addr[0, 0]  = &value

    //@print(addr[0,0].value)
    @assert(addr[0,0].value == 456)
}