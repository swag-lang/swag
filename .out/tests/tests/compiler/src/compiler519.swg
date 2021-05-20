//#[Swag.printbc]

#test
{
	var x, y = @{1,2,3}

	@assert(x.item0 == 1)
	@assert(x.item1 == 2)
	@assert(x.item2 == 3)
	@assert(y.item0 == 1)
	@assert(y.item1 == 2)
	@assert(y.item2 == 3)
}
