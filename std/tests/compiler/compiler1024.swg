

//#[swag.printbc]
#test
{
	x := @[1, 2, 3]
	ptr := &x
	@init(ptr, 2)
	@assert(x[0] == 0)
	@assert(x[1] == 0)
	@assert(x[2] == 3)
}