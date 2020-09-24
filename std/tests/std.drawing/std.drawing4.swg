#import "std.drawing"
using std.drawing

//#[swag.printbc]
#test
{
	var y: Color = 0xFFFF0000
    @print(cast(f64) y.toFloat().a)
    @print("\n")
    @print(cast(f64) y.toFloat().r)
    @print("\n")
    @print(cast(f64) y.toFloat().g)
    @print("\n")
    @print(cast(f64) y.toFloat().b)

    /*@print(cast(f64) y.toFloat().a)
    @print("\n")
    @print(cast(f64) y.toFloat().r)
    @print("\n")
    @print(cast(f64) y.toFloat().g)
    @print("\n")
    @print(cast(f64) y.toFloat().b)*/

    /*@assert(y.toFloat().a == 1)
    @assert(y.toFloat().r == 1)
    @assert(y.toFloat().g == 0)
    @assert(y.toFloat().b == 0)*/
}