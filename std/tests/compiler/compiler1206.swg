struct A
{
    v: u32
}

impl A
{
    #[swag.inline]
    func toto(s: *Self, v: u32)
    {
        s.v = v
    }
}

//#[swag.printbc]
#test
{
    var pt: A
    A.toto(pt, 666)
    @assert(pt.v == 666)
}