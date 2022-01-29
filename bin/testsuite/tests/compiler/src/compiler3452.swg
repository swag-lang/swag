#global testerror
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
    x := Vector{1, 2}

    v := @|x|()
    {
        @assert(x.x == 1)
        @assert(x.y == 2)
    }

    v()
}