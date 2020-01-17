struct Toto
{
    x, y: s32 = 10
}

impl Toto
{
	func(T) opCast(using self)->T
	{
		return x + y
	}
}

#test
{
	var x: Toto
	var y: s32 = cast(s32) x
	@assert(y == 20)
}