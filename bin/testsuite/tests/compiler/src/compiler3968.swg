var G: VV
var PC: bool

struct VV
{
    x, y, z: f32
}

impl VV
{
    mtd opPostCopy()
    {
        PC = true
    }
}

func toto()->const ref VV
{
    return &G
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    let x = ref toto()
    let y = x
    @assert(@typeof(x) == const ref VV)
    @assert(@typeof(y) == VV)
    @assert(PC == true)
}
