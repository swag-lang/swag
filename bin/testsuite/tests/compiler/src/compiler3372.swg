struct A { x: s32 }
var G = 0

impl A
{
    mtd opPostCopy()
    {
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    var a: [4] A
    var b: [4] A = a
    @assert(G == 4)
}
