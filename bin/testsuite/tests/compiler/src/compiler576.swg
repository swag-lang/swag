#global testerror "Err0533"

struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func opEquals(me, other: Point) -> s32
	{
		return 1
	}
}