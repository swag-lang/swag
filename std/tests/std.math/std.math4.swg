//#import "std"
#import "std.math"
using std.math

//#[swag.printbc]
#test
{
	var pt: Vector2(1, 2)
	var pt1: Vector2 = pt.normalize()
    //@assert(isEqualEpsilon(pt1.length(), 1))
}