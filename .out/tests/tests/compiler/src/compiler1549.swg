using Swag

var G = 0

struct Y
{

}

impl Y
{
    func toto(using self)
    {
        G += 1
    }
}

//#[Swag.printbc]
#test
{
    G = 0
    var y: Y
    var buffer: *Y = &y
    buffer[0].toto()
    @assert(G == 1)
}