#import "core"
#import "mthx"
using mthx, core.math

//#[swag.printbc]
#test
{
	var pt: Vector2{1, 2}
	pt1 := pt.normalized()
    @assert(isEqualEpsilon(pt1.length(), 1))
}