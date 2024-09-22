//#global skip
struct Vector2
{
    x, y: f32
}

impl Vector2
{
    mtd const length()->f32 => @sqrt((x * x) + (y * y))
}

#test
{
    let pt: Vector2{1, 1}
    let l   = pt.length()
    @assert(l > 0)
}