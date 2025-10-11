using Swag

struct A
{
    x:   s64 = 10
}

impl A
{
    #[Inline]
    func right(me)->s64
    {
        return me.x + me.x
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    @assert(pt.right() == 20)
}
