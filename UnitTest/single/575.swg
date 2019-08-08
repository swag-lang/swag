#module module575
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
}

#unittest error
impl Point
{
	func opEquals(self, other: Point, x: s32) -> bool
	{
		return self.x == other.x && self.y == other.y
	}
}