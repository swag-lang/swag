struct TT
{
    value:  s64 = 456
    value1: s64 = 456
    value2: s64 = 456
}

//#[Swag.PrintBc]
func C(x: ...)
{
    var ptr1 = cast(TT) x[0]
    //@print(cast(s64) ptr1.value)
    @assert(ptr1.value == 456)
}

#run
{
    var x: TT
    C(x)
}