//#[Swag.PrintBc]
#test
{
	var x = ["10", "20"]
	@init(&x, 2)("666")
	@assert(x[0] == "666")
	@assert(x[1] == "666")
}