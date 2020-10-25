#import "std"
using std.math

//#[swag.printbc]
#test
{
    var pt0, pt1, pt2: Point
    pt1.x = 1; pt1.y = 1
    pt2.x = 0.5; pt2.y = 1.5
    pt0 = pt1 + pt2
    @assert(pt0.x == 1.5)
    @assert(pt0.y == 2.5)
}