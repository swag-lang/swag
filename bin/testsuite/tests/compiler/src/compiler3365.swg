#global testerror "Err0102"
var G: s32

#[Swag.NoCopy]
struct AA { x: s32; }
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
    var v: [4] AA
    var v1: [4] AA
    v = v1
    @assert(G == 4)
}