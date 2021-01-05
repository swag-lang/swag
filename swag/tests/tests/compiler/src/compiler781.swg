#global testerror

struct Point
{
	x: s32
}

//#[swag.printbc]
#test
{
	var pt: Point
	var ptr: const *Point = &pt
	ptr.x = 0
}