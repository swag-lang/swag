


//#[swag.printbc]
#test
{
	c := @[1, 2, 3];
	d := @dataof(c)
	@assert(d[0] == 1)
	@assert(d[1] == 2)
	@assert(d[2] == 3)
}