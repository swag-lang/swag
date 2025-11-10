struct A
{
    x    = 1
}

var G = 0

impl A
{
    func opDrop(me)
    {
        G += 1
    }

    func opPostMove(me) {}

    func opPostCopy(me)
    {
        @assert(false)
    }
}

//#[Swag.PrintBc]
#test
{
    {
        var resultXX: [3] A
    }
    @assert(G == 3)
}
