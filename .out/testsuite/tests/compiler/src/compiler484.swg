


//#[Swag.printbc]
#test
{
	var res = T(0)
	@assert(res.item0 == 1)
	@assert(res.item1 == 2)
	@assert(res.item2 == 3)
}

//#[Swag.printbc]
func T(a: s32)-> {s32,s32,s32}
{
	return @{1,2,3}
}
