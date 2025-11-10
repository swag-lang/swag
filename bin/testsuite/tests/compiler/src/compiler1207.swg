struct A
{
    v:   u32
}

impl A
{
    #[Swag.Inline]
    func opEquals(me, _v: u32) => me.v == _v
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    pt.v = 555
    @assert(pt == 555)
}
