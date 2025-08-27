#test
{
    let x = 5.0
    @assert(5 == x)

    let z = "null"
    @assert(null != z)

    let y = 5 * x
    @assert(#typeof(y) == f32)
    @assert(y == 25.0)
}
