struct TT
{
    value:  s64 = 456
    value1: s64 = 456
    value2: s64 = 456
    value3: s64 = 456
    value4: s64 = 456
}

//#[Swag.PrintBc]
func D(x: ...)
{
    E(x[0])
}

//#[Swag.PrintBc]
func E(x: any)
{
    ptr := cast(TT) x
    //C(cast(TT) x)
    C(ptr)
}

//#[Swag.PrintBc]
func C(x: TT)
{
    //@print(cast(s64) x.value)
    @assert(x.value == 456)
}

#run
{
    var x: TT
    D(x)
}