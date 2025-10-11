/*struct A
{
    x: s64 = 100
}

impl A
{
    #[Swag.property, Swag.Inline]
    func right(me)->s64
    {
        return x
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    @assert(pt.right == 100)
}*/
