func toto() -> {x: s32, y: s32}
{
	return @{1, 2}
}

//#[Swag.PrintBc]
#test
{
	res := toto()
	@assert(res.x == 1)
	@assert(res.y == 2)
}
