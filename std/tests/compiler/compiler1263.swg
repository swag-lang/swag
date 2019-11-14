struct Toto
{
}

impl Toto
{
	func(T) opCast()->T
	{
	}
}

#test
{
	var x: Toto
	var y: s32 = x
}