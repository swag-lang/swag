#import "std"
using std.math

//#[swag.printbc]
#test
{
    {
        a := Rect{0, 0, 10, 8}
        b := Rect{5, 5, 15, 15}

        c := Rect.getUnion(a, b)
        @assert(c.x == 0)
        @assert(c.y == 0)
        @assert(c.width == 20)
        @assert(c.height == 20)
    }
}