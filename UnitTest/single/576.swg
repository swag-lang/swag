#module module576
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
}

#unittest error
impl Point
{
	func opEquals(self, other: Point) -> s32
	{
		return 1
	}
}