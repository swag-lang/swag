

//#[swag.printbc]
#test
{
	x := 10
	@init(&x)(666)
	@assert(x == 666)

	y := "10"
	@init(&y)("666")
	@assert(y == "666")
}