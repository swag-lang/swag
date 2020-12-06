#import "std"
#import "std_math"
using std_math, std.math

//#[swag.printbc]
#test
{
	var pt: Vector2{5, 100}
	pt.normalize()
    @assert(isEqualEpsilon(pt.length(), 1))
}