

//#[Swag.PrintBc]
#test
{
	x := 1
	ptr := &x
	@init(ptr)
	@assert(x == 0)
}