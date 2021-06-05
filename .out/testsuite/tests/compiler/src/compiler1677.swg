#global skip
struct TT
{
    value:  s64 = 456
    value1: s64 = 456
    value2: s64 = 456
}

func C(x: const &TT)
{
    @assert(x.value == 456)
}

#run
{
    var x: TT
    C(x)
}