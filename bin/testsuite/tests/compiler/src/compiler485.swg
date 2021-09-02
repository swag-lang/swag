//#[Swag.PrintBc]
#test
{
	var res = T()
	@assert(res.item0 == 1)
	@assert(res.item1 == 2)
	@assert(res.item2 == 3)
}

//#[Swag.PrintBc]
func T()-> {s64,s64,s64}
{
	return @{1's64,2's64,3's64}
}
