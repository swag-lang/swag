struct Point
{
    x: s64 = 666
}

interface IPrintable
{
    func print(self)
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    var itf = @mkinterface(&pt, @typeof(pt), IPrintable)
    @assert(itf == null)
}