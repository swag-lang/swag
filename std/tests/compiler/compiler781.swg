#module "compiler781"

struct Point
{
	x: s32
}

//#[swag.printbc]
#unittest error
#test
{
	var pt: Point
	var ptr: const *Point = &pt
	ptr.x = 0
}