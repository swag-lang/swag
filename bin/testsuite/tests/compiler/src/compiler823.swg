#global testerror

struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	let pt: Point(1, 2)
	pt = Point(1, 1)
}