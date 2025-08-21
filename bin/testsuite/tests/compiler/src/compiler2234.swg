struct A
{
    x = 1
}

var G = 0

impl A
{
    func opDrop(using me)
    {
        G += 1
    }

    func opPostMove(using me)
    {
    }

    func opPostCopy(using me)
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