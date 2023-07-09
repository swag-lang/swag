

//#[Swag.PrintBc]
#test
{
	var x = 10
	@init(&x)(666)
	@assert(x == 666)

	var y = "10"
	@init(&y)("666")
	@assert(y == "666")
}