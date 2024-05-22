struct Point
{
    x: s64
}

interface IPrintable
{
    func print(self);
}

impl IPrintable for Point
{
    func impl print(using self)
    {
        @print(x)
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: Point

    let type = @typeof(pt)
    visit itf: type.interfaces
    {
        /*if(itf.pointedType == IPrintable)
        {
            //var call: IPrintable = {&pt, &pt}
        }*/
    }
}