#import "std.math"
using std.math

//#[swag.printbc]
#test
{
	var pt: Vector2(1, 1)
	@print(cast(f64) pt.length())	
	@print("\n")
	@print(cast(f64) pt.dot(Vector2.One))	
}