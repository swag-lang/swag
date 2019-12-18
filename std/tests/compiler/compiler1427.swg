using swag

struct A
{
    x: s64
}

impl A
{
    #[inline]
    func right(using self)->s64
    {
        return self.x + self.x
    }
}

#[swag.printbc]
#test
{
    var pt: A
    @assert(pt.right() == 0)
}