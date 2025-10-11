#global testerror "Err0169"
struct Vector { x, y: f32 }

impl Vector
{
    mtd opDrop() {}
}

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var x = Vector{1, 2}

    var v = func|x|()
    {
        @assert(x.x == 1)
        @assert(x.y == 2)
    }

    v()
}
