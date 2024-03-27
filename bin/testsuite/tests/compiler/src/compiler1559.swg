struct A
{

}

var G = 0

impl A
{
    func opPostCopy(using self)
    {
    }

    func opDrop(using self)
    {
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0
    var x: A
    var y: A
    x = y
    @assert(G == 1)
    x = y
    @assert(G == 2)
    x =,nodrop y
    @assert(G == 2)
}