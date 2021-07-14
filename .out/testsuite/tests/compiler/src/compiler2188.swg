#[Swag.Inline]
func toFloat() -> {a: f32, r: f32, g: f32}//, b: f32}
{
    var result: retval = ?
    result.a = 1
    result.r = 2
    result.g = 3
    return result
}

//#[Swag.PrintBc, Swag.nooptim]
#test
{
    v := toFloat()
    @assert(v.a == 1)
    @assert(v.r == 2)
    @assert(v.g == 3)
}