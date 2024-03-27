var G = 0

//#[Swag.PrintBc]
func toto(v: closure(s32))
{
    v(5)
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    toto(func(x: s32)
    {
        G = x
    }
)

    @assert(G == 5)
}
