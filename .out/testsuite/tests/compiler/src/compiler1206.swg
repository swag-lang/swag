struct A
{
    v: u32
}

impl A
{
    #[Swag.Inline]
    func toto(s: *Self, _v: u32)
    {
        s.v = _v
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    A.toto(&pt, 666)
    @assert(pt.v == 666)
}