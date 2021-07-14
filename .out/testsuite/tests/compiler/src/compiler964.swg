

struct Point
{
	rank: s32
	value: string
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	pt = @{100, "1"}
	@assert(pt.rank == 100)
	@assert(pt.value == "1")
}