#global testerror "Err0599"

struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func opEquals(self, other: Point) -> s32
	{
		return 1
	}
}