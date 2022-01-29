struct Vector
{
    x, y: f32
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