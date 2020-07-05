struct TT
{
    value:  s64 = 456
}

//#[swag.printbc]
#run
{
    var addr:  [1] **s64
    var addr1: *s64
    var value: s64 = 666  

    addr[0]  = &addr1
    addr1 = &value

    @assert(addr[0,0,0] == 666)
}