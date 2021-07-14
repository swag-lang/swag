struct AA
{
    x = 555
}

impl AA
{
    func opIndexAffect(self, idx: uint, v: bool)
    {
        @assert(self.x == 555)
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
    ew := toto()
}