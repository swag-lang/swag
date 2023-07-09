var G: s32
var G1: s32

func toto()->ref s32
{
    return &G
}

func toto1()->ref s32
{
    return &G1
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var y = ref toto()
    y = 42
    @assert(G == 42)

    y = ref toto1()
    y = 5
    @assert(G1 == 5)
}
