#import "std"
#import "std.math"
using std.math

//#[swag.printbc]
#test
{
	var pt: Vector2(5, 100)
	var pt1: Vector2 = pt.normalize()
    @print(cast(f64) pt1.x)
    @print(" ")
    @print(cast(f64) pt1.y)
    //@assert(isEqualEpsilon(pt1.length(), 1))
}