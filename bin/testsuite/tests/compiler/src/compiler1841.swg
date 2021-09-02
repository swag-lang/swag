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

//#[Swag.PrintBc]
#test
{
    var pt: Point
    var an: any = pt

    itf := @mkinterface(@dataof(an), @kindof(an), IPrintable)
    @assert(itf != null)
    itf.print()
    @assert(G == 666)
}