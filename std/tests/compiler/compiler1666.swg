struct TT
{
    value:  s64 = 456
}

//#[swag.printbc]
func C(x: ...)
{
    ptr1 := cast(const *TT) x[0]
    @assert(ptr1.value == 456)
}

#run
{
    var x: TT
    C(x)
}