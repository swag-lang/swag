var G: s32

func toto()->const ref s32
{
    return &G
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    x := ref toto()
    y := &x
    @assert(@typeof(y) == const *s32)
    @assert(y == &G)
}
