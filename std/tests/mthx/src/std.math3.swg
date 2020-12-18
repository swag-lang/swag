#import "core"
#import "mthx"
using mthx, core.math

//#[swag.printbc]
#test
{
	var pt: Vector2{5, 100}
	pt.normalize()
    @assert(isEqualEpsilon(pt.length(), 1))
}