#module module579
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
}

#unittest error
impl Point
{
	func opEquals(x: s32, other: Point)->bool
	{
		return true;
	}
}