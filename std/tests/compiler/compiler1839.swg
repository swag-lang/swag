struct Point
{
    x: s64 = 666
}

interface IPrintable
{
    print: func(self)
}

impl IPrintable for Point
{
    func print(using self)
    {
        @print(x)
    }
}

#[swag.printbc]
#test
{
    var pt: Point
    var ty = @typeof(pt)
    itf := @interfaceof(&pt, ty, IPrintable)
    @assert(itf != null)
    itf.print()
}