struct TT
{
    value:  s64 = 456
    value1: s64 = 456
    value2: s64 = 456
}

func D(x: const &TT)
{
    //@print(cast(s64) x.value)
    @assert(x.value == 456)
}

//#[Swag.PrintBc]
func C(x: any)
{
    D(x)
}

#run
{
    var x: TT
    C(x)
}