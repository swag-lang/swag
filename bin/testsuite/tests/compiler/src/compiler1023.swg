

//#[Swag.PrintBc]
#test
{
	var x = 1
	let ptr = &x
	@init(ptr)
	@assert(x == 0)
}