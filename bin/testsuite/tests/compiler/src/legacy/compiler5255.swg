struct X {}

var G = 0

impl X
{
    mtd opDrop()
    {
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    for i in 1
    {
        var x: X

        switch i
        {
        case 0:
            var y: X
            continue
        }
    }

    @assert(G == 2)
}
