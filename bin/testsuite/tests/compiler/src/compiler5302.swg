struct A
{
    x: s32
}

var G0, G1: s32

impl A
{
    mtd opPostCopy()
    {
        G0 += 1
    }

    mtd opDrop()
    {
        G1 += 1
    }
}

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    {
        var a: A
        loop 2
        {
            var x = [a]
        }
    }

    @assert(G1 == G0 + 1)
}
