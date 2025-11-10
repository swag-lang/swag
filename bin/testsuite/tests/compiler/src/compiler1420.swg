/*struct A
{
    x, w: s64
}

impl A
{
    #[Swag.property]
    func right(me, t: s64)
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
}*/
