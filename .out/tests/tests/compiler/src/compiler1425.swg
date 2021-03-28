/*struct A
{
    x: s64 = 100
}

impl A
{
    #[swag.property, swag.inline]
    func right(using self)->s64
    {
        return x
    }
}

//#[swag.printbc]
#test
{
    var pt: A
    @assert(pt.right == 100)
}*/