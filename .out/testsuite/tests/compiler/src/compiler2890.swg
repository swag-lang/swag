struct Point
{
    x, y, z: f64
}

#[Swag.tls]
var pt: Point = @{1, 2, 3}

#test
{
    @assert(pt.x == 1)
    @assert(pt.y == 2)
    @assert(pt.z == 3)
}