struct A
{
    x: u32
}

impl A
{
    #[Swag.inline]
    func toto(s: *Self, v: u32)
    {
        s.x = v
    }
}

//#[Swag.printbc]
#test
{
    var pt: A
    A.toto(&pt, 666)
    @assert(pt.x == 666)
}