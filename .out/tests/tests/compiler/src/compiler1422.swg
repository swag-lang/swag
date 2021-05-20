/*struct A
{
    x, w: s64
}

impl A
{
    #[Swag.property]
    func right(using self)->s64
    {
        return x + w
    }

    #[Swag.property]
    func right(using self, t: s64)
    {
        w = t - x
    }
}

//#[Swag.printbc]
#test
{
    var pt: A
    pt.x = 1
    pt.w = 100
    @assert(pt.right == 101)
    pt.right = 10
    @assert(pt.right == 10)
    @assert(pt.w == 9)
}*/