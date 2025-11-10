struct A { x: s32 }
var G = 0

impl A
{
    mtd opPostCopy()
    {
        G += 10
    }

    mtd opPostMove()
    {
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    var a: [4] A
    var b: [4] A = #move a
    @assert(G == 4)
}
