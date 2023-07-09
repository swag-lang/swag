struct Vector
{
    x, y: f32
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