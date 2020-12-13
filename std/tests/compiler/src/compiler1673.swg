
#unittest error

struct TT
{
    value:  s64 = 456
}

//#[swag.printbc]
func C(x: ...)
{
    //@print(@typeof(x[0]).name)
    ptr1 := cast(const TT) x[0]
    ptr1.value = 456
}

#run
{
    var x: TT
    C(x)
}