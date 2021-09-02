func new() => @{1, 2}

//#[Swag.PrintBc]
#test
{
	a := new()
	@assert(a.item0 == 1)
	@assert(a.item1 == 2)

	@assert(new().item0 == 1)
	@assert(new().item1 == 2)
}