var G = 0

//#[Swag.PrintBc]
func toto(v: func||(s32))
{
    v(5)
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    toto(func||(x: s32)
    {
        G = x
    })

    @assert(G == 5)
}
