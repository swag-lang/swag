

func new() => @{1, 2}

//#[swag.printbc]
#test
{
	a := new()
	@assert(a.item0 == 1)
	@assert(a.item1 == 2)

	@assert(new().item0 == 1)
	@assert(new().item1 == 2)
}