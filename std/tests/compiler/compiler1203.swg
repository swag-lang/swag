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

#test
{
    var pt: A = 0xFFFF0000
    //@assert(pt.x == 0xFFFF0000)
}