struct Point
{
    x: s64 = 666
}

interface IPrintable
{
    print: func(self)
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    itf := @mkinterface(&pt, @typeof(pt), IPrintable)
    @assert(itf == null)
}