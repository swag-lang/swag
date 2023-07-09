var G: s32

func toto()->const ref s32
{
    return &G
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x = ref toto()
    var y = &x
    @assert(@typeof(y) == const *s32)
    @assert(y == &G)
}
