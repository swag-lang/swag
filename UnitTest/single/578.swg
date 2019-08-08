#module module578
//#unittest pass semantic

struct Point
{
	var x: s32
	var y: s32
}

type toto: s32

#unittest error
impl Point
{
	func opEquals(self, other: Point)->toto
	{
		return 1;
	}
}