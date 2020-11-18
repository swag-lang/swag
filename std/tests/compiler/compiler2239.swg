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

//#[swag.printbc]
func toto(v: s64)->Pt
{
    var result: retval = v
    return result
}

//#[swag.printbc]
#test
{
    res := toto(6)
    @assert(res.x == 6)
}