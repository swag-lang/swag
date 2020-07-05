struct TT
{
    value:  f64
}

//#[swag.printbc]
func A(x: any)
{
    ptr := cast(*TT) @dataof(x)
    @print(ptr.value)
    //@assert(ptr.value == -1.0)
}

func C(x: ...)
{
    A(x[0])
}

//#[swag.printbc]
#run
{
    x := TT(-1.0)
    C(x)
}