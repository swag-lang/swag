#import "std"
using std.math

//#[swag.printbc]
#test
{
    var pt0, pt1, pt2: Point
    pt1.x = 1; pt1.y = 1
    pt2.x = 0.5; pt2.y = 1.6
    pt0 = pt1 + pt2
    @assert(pt0.x == 1.5)
    @assert(pt0.y == 2.6)

    @assert(!pt0.isZeroEpsilon())

    var ptx: Point = pt0
    ptx.round()
    @assert(ptx.x == 2)
    @assert(ptx.y == 3)

    pty := pt0
    pty.trunc()
    @assert(pty.x == 1)
    @assert(pty.y == 2)
}