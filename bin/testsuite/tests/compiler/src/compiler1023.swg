

//#[Swag.PrintBc]
#test
{
	let x = 1
	let ptr = &x
	@init(ptr)
	@assert(x == 0)
}