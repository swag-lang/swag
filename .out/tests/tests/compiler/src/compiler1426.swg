/*struct A
{
    x: s64 = 100
}

impl A
{
    #[swag.property, swag.inline]
    func right(using self, v: s64)
    {
        x = v
    }
}

//#[swag.printbc]
#test
{
    var pt: A
    pt.right = 100
    @assert(pt.x == 100)
}*/