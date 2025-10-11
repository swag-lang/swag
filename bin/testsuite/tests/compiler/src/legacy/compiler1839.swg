struct Point
{
    x:   s64 = 666
}

var G: s64 = 0

interface IPrintable
{
    func print(me);
}

impl IPrintable for Point
{
    func impl print(me)
    {
        G = 666
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    let ty  = #typeof(pt)
    let itf = @mkinterface(&pt, ty, IPrintable)
    @assert(itf != null)
    itf.print()
    @assert(G == 666)
}
