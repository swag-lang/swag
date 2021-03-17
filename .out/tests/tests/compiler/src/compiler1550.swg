//using swag

struct(T) X
{
    buffer: *T
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

//#[swag.printbc]
#test
{
    var y: Y
    var x: X'Y
    x.buffer = &y
    x.buffer[0].toto()
}