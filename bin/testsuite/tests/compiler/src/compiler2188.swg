#[Swag.Inline]
func toFloat()->{a: f32, r: f32, g: f32}
{
    var result: retval = undefined
    result.a = 1
    result.r = 2
    result.g = 3
    return result
}

//#[Swag.PrintBc, Swag.nooptim]
#test
{
    var v = toFloat()
    @assert(v.a == 1)
    @assert(v.r == 2)
    @assert(v.g == 3)
}
