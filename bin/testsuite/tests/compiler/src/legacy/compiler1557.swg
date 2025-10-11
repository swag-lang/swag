//using Swag

struct(TTX) A {}

var G = 0

impl A
{
    /*func fct(me)
    {
        var ptr : TTX
    }*/

    func opDrop(me)
    {
        G = 1
        var ptr: TTX
        @assert(#typeof(TTX) == #typeof(s32))
    }
}

//#[Swag.PrintBc]
#test
{
    G = 0

    {
        var x: A's32
        //x.fct()
    }

    @assert(G == 1)
}
