#import "std.drawing"
using std.drawing

//#[swag.printbc]
#test
{
	x := Argb.Red
	var y: Color = cast(u32) Argb.Red
    @assert(y.a == 255)
    @assert(y.r == 255)
    @assert(y.g == 0)
    @assert(y.b == 0)
}