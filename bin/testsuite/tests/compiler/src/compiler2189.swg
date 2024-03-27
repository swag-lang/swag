struct P
{
    a: u8
}

impl P
{
    #[Swag.Inline]
    func opAffect(using self, v: u8)
    {
        a = v
    }

    #[Swag.Inline]
    func toFloat(using self)->{a: f32}
    {
        var result: retval = undefined
        result.a = self.a / 255'f32
        return result
    }
}

//#[Swag.PrintBc]
//#[Swag.nooptim]
#test
{
    var x: P = 255'u8
    var v  = x.toFloat()
    @assert(v.a == 1)
}
