struct A
{
    x = 1
}

var G = 0

impl A
{
    func opDrop(using self)
    {
        G += 1
    }

    func opPostMove(using self)
    {
    }

    func opPostCopy(using self)
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