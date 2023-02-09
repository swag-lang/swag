//#global skip
using Swag

var G = 0

#test
{
	x := [1, 2]
	//@print(@typeof(x).name)
	ptr := &x
	y := ref &ptr
	//@print(@typeof(y).name)
	z := y[0]
}
