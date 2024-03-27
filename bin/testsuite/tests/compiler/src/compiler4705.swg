var G  = 0
var G1 = 1

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    //@breakpoint()

    let firstIdx = G
    let lastIdx  = G1

    loop i: firstIdx to lastIdx
    {
        //@print(firstIdx, " ", lastIdx, " ", i, "\n")
        @assert(i <= 1)
    }
}
