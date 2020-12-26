struct A
{
    x, w: s64
}

impl A
{
    #[swag.property]
    func right(using self, t: s64)
    {
        x = t
    }
}

#test
{
    var pt: A
    pt.x = 1
    pt.w = 10
    pt.right(10)
    @assert(pt.x == 10)
}