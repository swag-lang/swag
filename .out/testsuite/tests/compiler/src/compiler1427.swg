using Swag

struct A
{
    x: s64 = 10
}

impl A
{
    #[inline]
    func right(self)->s64
    {
        return self.x + self.x
    }
}

//#[Swag.printbc]
#test
{
    var pt: A
    @assert(pt.right() == 20)
}