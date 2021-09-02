

//#[Swag.PrintBc]
#test
{
	x := @["10", "20", "30"]
	count := 2
	@init(&x, count)("666")
	@assert(x[0] == "666")
	@assert(x[1] == "666")
	@assert(x[2] == "30")
}