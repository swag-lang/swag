#global testerror

struct Point
{
	x, y: s32
}
//#[swag.printbc]
#test
{
	var pt : Point
	ptr := &pt
	init(ptr)(10, true)
}