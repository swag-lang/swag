struct AA {}

impl AA
{
    func titi(me)->s32
    {
        return 820856226
    }
}

struct BB
{
    v:   AA
}

impl BB
{
    #[Swag.Inline]
    func toto(me)->s32
    {
        return me.v.titi()
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
