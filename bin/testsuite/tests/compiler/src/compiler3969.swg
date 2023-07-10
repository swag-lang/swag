var G: s32

func toto()->ref s32
{
    return &G
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    let x = ref toto()
    var y = ref x
    @assert(@typeof(x) == #type ref s32)
    @assert(@typeof(y) == #type ref s32)

    y = 5
    @assert(G == 5)
}
