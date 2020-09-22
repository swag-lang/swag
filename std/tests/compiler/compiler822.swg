

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	let pt: Point{1, 2}
	pt.x = 0
}