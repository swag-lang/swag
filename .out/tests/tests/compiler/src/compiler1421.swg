/*struct A
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

//#[swag.printbc]
#test
{
    var pt: A
    pt.x = 1
    pt.w = 10
    pt.right = 100
    @assert(pt.x == 100)
    //@assert(pt.right == 100)
}*/