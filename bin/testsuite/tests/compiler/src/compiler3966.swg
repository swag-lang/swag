#global testerror "Err0227"

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

func toto()->VV
{
    return G
}

//#[Swag.PrintBc]
#test
{
    var x = #ref toto()
}
