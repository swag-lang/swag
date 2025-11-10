var G:  VV
var PC: bool

struct VV { x, y, z: f32 }

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

//#[Swag.PrintBc]
#test
{
    let x = toto()
    @assert(#typeof(x) == VV)
    @assert(PC)
}
