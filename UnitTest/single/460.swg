#module module460
//#unittest pass syntax

//#[swag.printbc]
#test
{
	c := [1, 2, 3];
	d := @prop(c, data)
	@assert(d[0] == 1)
	@assert(d[1] == 2)
	@assert(d[2] == 3)
}