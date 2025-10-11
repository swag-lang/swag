struct AA
{
    x    = 555
}

impl AA
{
    func opIndexAffect(me, idx: u64, v: bool)
    {
        @assert(me.x == 555)
    }
}

//#[Swag.PrintBc]
func toto()->AA
{
    var result: retval
    result[0] = true
    return result
}
#test
{
    var ew = toto()
}
