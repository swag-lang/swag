var G: s32

func toto()->ref s32
{
    return &G
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    x := kref toto()
    y := kref x
    @assert(@typeof(x) == ref s32)
    @assert(@typeof(y) == ref s32)

    y = 5
    @assert(G == 5)
}
