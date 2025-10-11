//#global skip

struct(T) A
{
    fn:  T
}

impl A
{
    mtd call(params: ...)
    {
        me.fn(params[0], params[1])
    }
}

func lambda(x, y: s32)
{
    @assert(x == 1)
    @assert(y == 2)
}

#test
{
    var x: A'func(s32, s32)
    x.fn = &lambda
    x.call(1, 2)
}
