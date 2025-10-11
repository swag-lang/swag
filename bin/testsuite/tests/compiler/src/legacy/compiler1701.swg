interface Print
{
    func toto(me)->s32;
}

struct A
{
    x:   f64 = 666
}

impl Print for A
{
    func impl toto(me)->s32
    {
        @assert(me.x == 666)
        return 667
    }
}

func callAny(value: any)
{
    let ptr = cast(A) value
    let itf = cast(Print) ptr
    @assert(itf.toto() == 667)
    @assert(#typeof(itf).fullname == "__compiler1701.Print")
}

#test
{
    var str: A
    callAny(str)
}
