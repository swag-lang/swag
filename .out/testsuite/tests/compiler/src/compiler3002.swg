struct Vector2
{
    x: f32
}

//#[Swag.PrintBc, Swag.optim("", false)]
#test
{
    size := 10's32
    v := Vector2{1.0 + size}
    @assert(v.x == 11.0)
}