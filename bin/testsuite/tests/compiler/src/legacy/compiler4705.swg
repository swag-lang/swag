var G  = 0
var G1 = 1

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    //@breakpoint()

    let firstIdx = G
    let lastIdx  = G1

    for i in firstIdx to lastIdx
    {
        //@print(firstIdx, " ", lastIdx, " ", i, "\n")
        @assert(i <= 1)
    }
}
