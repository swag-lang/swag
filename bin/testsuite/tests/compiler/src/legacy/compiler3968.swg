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

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let x = #ref toto()
    let y = x
    @assert(#typeof(x) == const &VV)
    @assert(#typeof(y) == VV)
    @assert(PC == true)
}
