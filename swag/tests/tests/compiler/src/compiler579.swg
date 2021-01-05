#global testerror

struct Point
{
	x: s32
	y: s32
}

impl Point
{
	func opEquals(x: s32, other: Point)->bool
	{
		return true;
	}
}