struct Pt
{
    x: s64 = 1
}

impl Pt
{
    func opAffect(using self, other: s64)
    {
        x = other
    }
}

//#[Swag.PrintBc]
func toto(v: s64)->Pt
{
    var result: retval = v
    return result
}

//#[Swag.PrintBc]
#test
{
    res := toto(6)
    @assert(res.x == 6)
}