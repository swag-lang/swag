

//#[Swag.PrintBc]
#test
{
	var x = 1
	var ptr = &x
	@init(ptr)
	@assert(x == 0)
}