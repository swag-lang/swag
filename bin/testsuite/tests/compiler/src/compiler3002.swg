struct Vector2
{
    x:   f32
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let size = 10's32
    var v    = Vector2{1.0 + size}
    @assert(v.x == 11.0)
}
