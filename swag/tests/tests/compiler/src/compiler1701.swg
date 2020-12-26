interface Print
{
    toto: func(self)->s32
}

struct A
{
    x: f64 = 666
}

impl Print for A
{
    func toto(using self)->s32
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
    @assert(@typeof(itf).name == "__compiler1701.Print")
}

#test
{
    var str: A
    callAny(str)
}