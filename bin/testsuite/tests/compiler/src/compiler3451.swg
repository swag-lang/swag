struct Vector { x, y: f32 }

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let x = Vector{1, 2}

    let v = func|x|()
    {
        @assert(x.x == 1)
        @assert(x.y == 2)
    }

    v()
}
