struct A
{
    x: s32 = 555
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
    var y: A = x
    @assert(G == 0)
}