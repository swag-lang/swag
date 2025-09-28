/*
#global testerror

struct TT
{
    value:  s64 = 456
}

//#[Swag.PrintBc]
func C(x: ...)
{
    //var ptr = @kindof(x[0])
    //@print(ptr.name)
    var ptr1 = cast(*TT) x[0]
    //@print(ptr1.value)
    //var ptr1 = cast(*s64) @dataof(x[0])
}

#run
{
    var x: TT
    C(x)
    //var y: s64 = 666
    //C(y)
}*/