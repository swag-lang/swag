interface Print
{
    func toto(self)->s32
}

struct A
{
    x: f64 = 666
}

impl Print for A
{
    func impl toto(using self)->s32
    {
        @assert(x == 666)
        return 667
    }
}

func callAny(value: any)
{
    ptr := cast(A) value
    itf := cast(Print) ptr
    @assert(itf.toto() == 667)
    @assert(@typeof(itf).fullname == "__compiler1701.Print")
}

#test
{
    var str: A
    callAny(str)
}