struct(T) Point
{
    x: T
    y: T
}

impl Point
{
    mtd gx() -> ref T
    {
        return ref &x
    }
}

#test
{
    var p = Point{ x: 5's32, y: 10 }
    @assert(p.gx() == 5)
}