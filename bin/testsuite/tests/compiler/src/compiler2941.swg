struct AA
{
}

impl AA
{
    func titi(using self)->s32
    {
        return 820856226
    }
}

struct BB
{
    v: AA
}

impl BB
{
    #[Swag.Inline]
    func toto(using self)->s32
    {
        return v.titi()
    }
}

func ptr(x: ...)
{
    let v = cast(s32) x[0]
    @assert(v == 820856226)
}

#test
{
    var x: BB
    ptr(x.toto())
}