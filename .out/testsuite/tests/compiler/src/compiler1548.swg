using Swag

struct(T) X
{
    buffer: *T
}

/*impl X
{
    func titi(using self)
    {
        buffer[0].toto()
    }
}*/

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
    x.buffer = &y
    //x.buffer[0].toto()
    Y.toto(x.buffer[0])
    //x.titi()
}