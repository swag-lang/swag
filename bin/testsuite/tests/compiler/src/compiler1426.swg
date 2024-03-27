/*struct A
{
    x: s64 = 100
}

impl A
{
    #[Swag.property, Swag.Inline]
    func right(using self, v: s64)
    {
        x = v
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    pt.right = 100
    @assert(pt.x == 100)
}*/