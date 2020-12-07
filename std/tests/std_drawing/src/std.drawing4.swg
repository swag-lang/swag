#import "std_drawing"
using std_drawing

//#[swag.printbc]
#test
{
	var y: Color = 0xFFFF0000
    @assert(y.toFloat().a == 1)
    @assert(y.toFloat().r == 1)
    @assert(y.toFloat().g == 0)
    @assert(y.toFloat().b == 0)
}