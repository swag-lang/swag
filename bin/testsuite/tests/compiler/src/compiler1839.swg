struct Point
{
    x: s64 = 666
}

var G: s64 = 0

interface IPrintable
{
    func print(self);
}

impl IPrintable for Point
{
    func impl print(using self)
    {
        G = 666
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    let ty = #typeof(pt)
    let itf = @mkinterface(&pt, ty, IPrintable)
    @assert(itf != null)
    itf.print()
    @assert(G == 666)
}