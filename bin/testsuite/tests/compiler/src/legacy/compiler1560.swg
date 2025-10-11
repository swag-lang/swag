struct A
{
    x:   s32 = 555
}

var G = 0

impl A
{
    func opPostCopy(me) {}

    func opDrop(me)
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
