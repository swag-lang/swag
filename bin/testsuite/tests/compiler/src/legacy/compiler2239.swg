struct Pt
{
    x:   s64 = 1
}

impl Pt
{
    func opAffect(me, other: s64)
    {
        me.x = other
    }
}

//#[Swag.PrintBc]
func toto(v: s64)->Pt
{
    let result: retval = v
    return result
}

//#[Swag.PrintBc]
#test
{
    let res = toto(6)
    @assert(res.x == 6)
}
