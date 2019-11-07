#import "std"
#import "std.math"
using std.math

//#[swag.printbc]
#test
{
	var pt: Vector2(5, 100)
	pt.makeNormalize()
    @assert(isEqualEpsilon(pt.length(), 1))
}