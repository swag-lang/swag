//#[Swag.PrintBc]
func toto(y: s32)->f32
{
    var x = cast(f32) y
    x *= 3
    return x
}

#test
{
    let y = toto(3)
    @assert(y == 9.0)
}
