#global testerror

struct Point
{
	x, y: s32
}

//#[Swag.PrintBc]
#test
{
	var pt : Point
	var ptr = &pt
	@init(ptr)(10, true)
}