struct A
{
    x: u32
}

impl A
{
    #[swag.complete, swag.inline]
    func opAffect(self, v: u32)
    {
        self.x = v
    }
}

//#[swag.printbc]
#test
{
    var pt: A
    pt = 0xFFFF0000
    @assert(pt.x == 0xFFFF0000)
}