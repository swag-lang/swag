struct Point
{
    x:   s64
}

interface IPrintable
{
    func print(me);
}

impl IPrintable for Point
{
    func impl print(me)
    {
        @print(me.x)
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: Point

    let type = #typeof(pt)
    foreach itf in type.interfaces
    {
        /*if(itf.pointedType == IPrintable)
        {
            //var call: IPrintable = {&pt, &pt}
        }*/
    }
}
