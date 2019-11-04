struct A
{
    v: u32
}

impl A
{
    #[swag.inline]
    func opEquals(self, v: u32) => self.v == v
}

//#[swag.printbc]
#test
{
    var pt: A
    pt.v = 555
    @assert(pt == 555)
}