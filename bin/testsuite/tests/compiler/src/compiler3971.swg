var G: s32

func toto()->ref s32
{
    return &G
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x = ref toto()
    var y = &x
    @assert(@typeof(y) == *s32)
    @assert(y == &G)
}
