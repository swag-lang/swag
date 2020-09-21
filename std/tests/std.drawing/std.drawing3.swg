#import "std.drawing"
using std.drawing

//#[swag.printbc]
#test
{
	var y: Color = "Aqua"
    @assert(y.argb == Argb.Aqua)
    @assert(y == Argb.Aqua)
}