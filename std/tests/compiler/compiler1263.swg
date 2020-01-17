struct Toto
{
}

impl Toto
{
	func(T) opCast(using self)->T
	{
		return 666
	}
}

#test
{
	var x: Toto
	var y: s32 = cast(s32) x
	@assert(y == 666)
}