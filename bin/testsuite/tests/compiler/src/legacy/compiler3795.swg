struct X {}

impl X
{
    mtd opDrop()
    {
        G += 1
    }
}

var G: s32

#[Swag.Mixin]
func toto() throw
{
}

//#[Swag.PrintBc]
#test
{
    var x: X

    G += 1
    try toto()
    @assert(G == 1)
    G += 1
}
