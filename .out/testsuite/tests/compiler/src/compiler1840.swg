struct Point
{
    x: s64 = 666
}

var G: s64 = 0

interface IPrintable
{
    print: func(self)
}

impl IPrintable for Point
{
    func print(using self)
    {
        G = 666
    }
}

//#[Swag.printbc]
#test
{
    var pt: Point
    itf := @mkinterface(&pt, @typeof(pt), IPrintable)
    @assert(itf != null)
    itf.print()
    @assert(G == 666)
}