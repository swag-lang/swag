struct A
{
    x: u32
}

impl A
{
    #[Swag.Complete, Swag.inline]
    func opAffect(self, v: u32)
    {
        self.x = v
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    pt = 0xFFFF0000
    @assert(pt.x == 0xFFFF0000)
}