var G: s32

struct AA { x: s32 }
impl AA
{
    mtd opDrop()
    {
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    var v:  [4] AA
    var v1: [4] AA
    v = v1
    @assert(G == 4)
}
