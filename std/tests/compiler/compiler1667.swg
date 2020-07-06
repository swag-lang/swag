/*#module compiler1667
#unittest error

struct TT
{
    value:  s64 = 456
}

//#[swag.printbc]
func C(x: ...)
{
    ptr1 := cast(TT) x[0]
    ptr1.value = 456
}

#run
{
    var x: TT
    C(x)
}*/