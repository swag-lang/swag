struct A
{
    x:   u32
}

impl A
{
    #[Swag.Complete, Swag.Inline]
    func opAffect(me, v: u32)
    {
        me.x = v
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    pt = 0xFFFF0000
    @assert(pt.x == 0xFFFF0000)
}
