
using Core.Math

//#[Swag.printbc]
#test
{
    {
        a := Rectangle{0, 0, 10, 8}
        b := Rectangle{5, 5, 15, 15}

        c := Rectangle.getUnion(a, b)
        @assert(c.x == 0)
        @assert(c.y == 0)
        @assert(c.width == 20)
        @assert(c.height == 20)
    }
}