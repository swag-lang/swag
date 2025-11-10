struct(T) Point
{
    x:     T
    y:     T
}

impl Point
{
    mtd gx()->&T
    {
        return &me.x
    }
}

#test
{
    var p = Point{x: 5's32, y: 10}
    @assert(p.gx() == 5)
}
