#global testerror "Err0090"

struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	const pt: Point{1, 2}
	pt = Point{1, 1}
}