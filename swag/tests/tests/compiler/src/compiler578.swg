


struct Point
{
	x: s32
	y: s32
}

alias toto = s32

#unittest error
impl Point
{
	func opEquals(self, other: Point)->toto
	{
		return 1;
	}
}