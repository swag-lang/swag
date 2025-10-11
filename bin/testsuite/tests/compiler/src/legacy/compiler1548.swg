using Swag

struct(T) X
{
    buffer: [*] T
}

/*impl X
{
    func titi(me)
    {
        buffer[0].toto()
    }
}*/

struct Y {}

impl Y
{
    func toto(me) {}
}

//#[Swag.PrintBc]
#test
{
    var y: Y
    var x: X'Y
    x.buffer = cast([*] Y) &y
    //x.buffer[0].toto()
    Y.toto(x.buffer[0])
    //x.titi()
}
