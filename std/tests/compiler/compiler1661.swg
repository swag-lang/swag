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
    //addr1[0] = &value

    //@print(addr[0].value)
}