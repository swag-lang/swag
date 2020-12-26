

struct Point
{
	x: s32
	y: s32
}

#unittest error
//#[swag.printbc]
#test
{
	let pt: Point(1, 2)
	pt = Point(1, 1)
}