/*#module "compiler1660"
#unittest error

struct TT
{
    value:  s64 = 456
}

//#[swag.printbc]
func C(x: ...)
{
    //ptr := @kindof(x[0])   
    //@print(ptr.name)
    ptr1 := cast(*TT) x[0]
    //@print(ptr1.value)
    //ptr1 := cast(*s64) @dataof(x[0])
}

#run
{
    var x: TT
    C(x)
    //var y: s64 = 666
    //C(y)
}*/