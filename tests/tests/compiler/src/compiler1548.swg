using swag

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

//#[swag.printbc]
#test
{
    var y: Y
    var x: X'Y
    x.buffer = &y
    //x.buffer[0].toto()
    Y.toto(x.buffer[0])
    //x.titi()
}