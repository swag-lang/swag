struct Point
{
    x:   s64 = 666
}

interface IPrintable
{
    func print(me);
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    let itf = @mkinterface(&pt, #typeof(pt), IPrintable)
    @assert(itf == null)
}
