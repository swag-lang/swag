struct P
{
    a: u8
}

impl P
{
    #[Swag.Inline]
    func opAffect(using me, v: u8)
    {
        a = v
    }

    #[Swag.Inline]
    func toFloat(using me) -> {a: f32}
    {
        var result: retval = undefined
        result.a = me.a / 255'f32
        return result
    }
}

//#[Swag.PrintBc]
//#[Swag.nooptim]
#test
{
    var x: P = 255'u8
    var v = x.toFloat()
    @assert(v.a == 1)
}