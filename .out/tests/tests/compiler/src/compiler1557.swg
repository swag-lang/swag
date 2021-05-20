//using Swag

struct(TTX) A
{
}

var G = 0

impl A
{
    /*func fct(using self)
    {
        var ptr : TTX
    }*/

    func opDrop(using self)
    {
        G = 1
        var ptr : TTX
        @assert(@typeof(TTX) == @typeof(s32))
    }
}

//#[Swag.printbc]
#test
{
    G = 0

    {
        var x: A's32
        //x.fct()
    }

    @assert(G == 1)
}