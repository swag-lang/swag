struct Vec2
{
    buffer: [2] f32
}

impl Vec2
{
    mtd const opIndex(idx: u64)->f32
    {
        return me.buffer[idx]
    }
}

//#[Swag.PrintBc]
func dot(x: const &Vec2)
{
    var y: f32
    y = x[0]
    @assert(y == 1.5)
    @assert(x[1] == 2.0)
}

//#[Swag.PrintBc]
#test
{
    var x = Vec2{[1.5, 2]}
    //@print(x[0], " ", x[1], "\n")
    dot(x)
}
