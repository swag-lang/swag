using Swag

struct A
{
    x: s64 = 10
}

impl A
{
    #[Inline]
    func right(self)->s64
    {
        return self.x + self.x
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    @assert(pt.right() == 20)
}