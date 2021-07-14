#global testerror

struct Point
{
	x, y: s32
}
//#[Swag.PrintBc]
#test
{
	var pt : Point
	ptr := &pt
	init(ptr)(10, true)
}