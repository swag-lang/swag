#global testerror

struct Point
{
	x, y: s32
}
//#[Swag.printbc]
#test
{
	var pt : Point
	ptr := &pt
	init(ptr)(10, true)
}