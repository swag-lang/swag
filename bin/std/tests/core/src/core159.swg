using Core.Math

//#[Swag.PrintBc]
#test
{
    {
        let a = Rectangle{0, 0, 10, 8}
        let b = Rectangle{5, 5, 15, 15}

        let c = Rectangle.getUnion(a, b)
        @assert(c.x == 0)
        @assert(c.y == 0)
        @assert(c.width == 20)
        @assert(c.height == 20)
    }
}
