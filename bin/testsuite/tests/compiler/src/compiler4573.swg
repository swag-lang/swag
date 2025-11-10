struct Vec2 { x, y: f32 }

#test
{
    let x = [1.0, 2.0]
    let y = cast([..] f32) x
    @assert(@countof(y) == 2)
    let z = cast([..] Vec2) y
    @assert(@countof(z) == 1)
    @assert(z[0].x == 1)
    @assert(z[0].y == 2)
}
