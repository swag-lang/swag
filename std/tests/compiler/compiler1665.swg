#module "compiler1665"
#unittest error

struct TT
{
    value:  s64 = 456
}

//#[swag.printbc]
func C(x: ...)
{
    ptr1 := cast(TT) x[0]
    ptr1.value = 0
}

#run
{
    var x: TT
    C(x)
}