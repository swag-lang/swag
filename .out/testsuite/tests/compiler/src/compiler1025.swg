//#[Swag.PrintBc]
#test
{
	x := @[1, 2, 3]
	ptr := &x
	count := 3
	@init(ptr, count)
	@assert(x[0] == 0)
	@assert(x[1] == 0)
	@assert(x[2] == 0)
}