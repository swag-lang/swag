#global testerror "Err0225"
struct Vector
{
    x, y: f32
}

impl Vector
{
    mtd opDrop() {}
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x = Vector{1, 2}

    var v = closure|x|()
    {
        @assert(x.x == 1)
        @assert(x.y == 2)
    }

    v()
}