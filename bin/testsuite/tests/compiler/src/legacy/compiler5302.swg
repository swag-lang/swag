struct A
{
    x:   s32
}

var G0, G1: s32

impl A
{
    mtd opPostCopy()
    {
        G0 += 1
        //@print("copy ", me, "\n")
    }

    mtd opDrop()
    {
        G1 += 1
        //@print("drop ", me, "\n")
    }
}

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    {
        var a: A
        for 2
        {
            var x = [a]
        }
    }

    @assert(G1 == G0 + 1)
}
