struct A
{
    v: u32
}

impl A
{
    #[Swag.inline]
    func opEquals(self, _v: u32) => self.v == _v
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    pt.v = 555
    @assert(pt == 555)
}