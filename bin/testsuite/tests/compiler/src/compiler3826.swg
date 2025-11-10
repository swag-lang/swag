struct A {}

var G = 0
impl A
{
    mtd opDrop()
    {
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    {
        var x:    [2] A
        var count = 1
        count -= 1
        @drop(&x[0], count)
    }

    @assert(G == 2)
}
