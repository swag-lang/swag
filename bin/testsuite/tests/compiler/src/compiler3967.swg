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

func toto()->const &VV
{
    return &G
}

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    let x: const &VV = toto()
    let y = x
    @assert(#typeof(x) == const &VV)
    @assert(#typeof(y) == VV)
    @assert(PC == true)
}
