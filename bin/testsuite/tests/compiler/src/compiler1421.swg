/*struct A
{
    x, w: s64
}

impl A
{
    #[Swag.property]
    func right(using self, t: s64)
    {
        x = t
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    pt.x = 1
    pt.w = 10
    pt.right = 100
    @assert(pt.x == 100)
    //@assert(pt.right == 100)
}*/