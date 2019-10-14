

struct Point
{
	x, y: s32
}
//#[swag.printbc]
#unittest error
#test
{
	var pt : Point
	ptr := &pt
	init(ptr)(10, true)
}