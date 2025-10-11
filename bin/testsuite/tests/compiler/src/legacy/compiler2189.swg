struct P
{
    a:   u8
}

impl P
{
    #[Swag.Inline]
    func opAffect(me, v: u8)
    {
        me.a = v
    }

    #[Swag.Inline]
    func toFloat(const me)->{ a: f32 }
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
    let x: P = 255'u8
    let v  = x.toFloat()
    @assert(v.a == 1)
}
