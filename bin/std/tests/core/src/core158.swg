using Core.Math

//#[Swag.PrintBc]
#test
{
    {
        var a = Rectangle{0, 0, 10, 8}
        var b = Rectangle{5, 5, 15, 15}
        @assert(a.intersectWith(b))
        @assert(b.intersectWith(a))

        var c = Rectangle.intersect(a, b)
        @assert(c.x == 5)
        @assert(c.y == 5)
        @assert(c.width == 5)
        @assert(c.height == 3)
    }

    {
        var a = Rectangle{0, 0, 10, 8}
        var b = Rectangle{5, 5, 15, 15}
        a.intersect(b)
        @assert(a.x == 5)
        @assert(a.y == 5)
        @assert(a.width == 5)
        @assert(a.height == 3)
    }
}
