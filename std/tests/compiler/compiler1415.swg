struct A
{
    x, w: s64
}

impl A
{
    #[swag.property]
    func right(using self, value: s32)
    {
        w = value - x
    }
}

#test
{
    var pt: A
    pt.x = 1
    pt.w = 10
    A.right(pt) = 10
}