struct TT
{
    value:  f64
    bits:   s64 = 32
}

func A(x: any)
{
    ptr := cast(TT) x
    @assert(ptr.value == -1.0)
    @assert(ptr.bits == 32)
}

//#[swag.printbc]
#run
{
    A(TT(-1.0))
}