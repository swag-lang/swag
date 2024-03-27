//using Swag

struct(T) X
{
    buffer: ^T
}

struct Y
{
}

impl Y
{
    func toto(using self)
    {
    }
}

//#[Swag.PrintBc]
#test
{
    var y: Y
    var x: X'Y
    x.buffer = cast(^Y) &y
    x.buffer[0].toto()
}
