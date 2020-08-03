struct Point
{
    x: s64
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

//#[swag.printbc]
#test
{
    var pt: Point

    type := @typeof(pt)
    visit itf: type.interfaces
    {
        /*if(itf.pointedType == IPrintable)
        {
            //var call: IPrintable = @{&pt, &pt}
        }*/
    }
}