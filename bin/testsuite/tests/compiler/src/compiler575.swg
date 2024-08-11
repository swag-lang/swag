#global testerror "Err0544"

struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func opEquals(self, other: Point, x: s32) -> bool
	{
		return self.x == other.x and self.y == other.y
	}
}