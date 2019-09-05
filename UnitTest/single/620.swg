#module module620
//#unittest pass semantic

struct Point
{
	x: s32
}

impl Point
{
	func toto(x: Self)
	{
		x.x = 0
	}
}