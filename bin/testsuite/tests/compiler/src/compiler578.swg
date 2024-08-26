#global testerror "Err0570"

struct Point
{
	x: s32
	y: s32
}

typealias toto = s32

impl Point
{
	func opEquals(self, other: Point)->toto
	{
		return 1;
	}
}