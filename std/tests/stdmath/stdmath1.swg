#import "stdmath"
using std

//#[swag.printbc]
#test
{
	x := math.cos(1.0'f32)
	y := math.sqrt(4.0'f32)
	@assert(y == 2)
}