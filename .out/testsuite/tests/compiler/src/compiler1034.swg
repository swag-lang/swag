

//#[Swag.printbc]
#test
{
	x := @["10", "20", "30"]
	count := 2
	@init(&x, count)
	@assert(x[0] == null)
	@assert(x[1] == null)
	@assert(x[2] == "30")
}