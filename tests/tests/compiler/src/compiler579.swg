


struct Point
{
	x: s32
	y: s32
}

#unittest error
impl Point
{
	func opEquals(x: s32, other: Point)->bool
	{
		return true;
	}
}