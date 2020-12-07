#module "compiler576"


struct Point
{
	x: s32
	y: s32
}

#unittest error
impl Point
{
	func opEquals(self, other: Point) -> s32
	{
		return 1
	}
}