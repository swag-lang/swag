using Swag

var G = 0

struct Y {}

impl Y
{
    func toto(me)
    {
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0
    var y:      Y
    let buffer: [*] Y = cast([*] Y) &y
    buffer[0].toto()
    @assert(G == 1)
}
