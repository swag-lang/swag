#module module459
//#unittest pass syntax

//#[swag.printbytecode]
#test
{
	var c = [1, 2, 3];
	var d: [..] s32 = [ &c[0], 3 ]
	@assert(@prop(d, count) == 3)
	@assert(c[0] == 1)
	@assert(c[1] == 2)
	@assert(c[2] == 3)
}