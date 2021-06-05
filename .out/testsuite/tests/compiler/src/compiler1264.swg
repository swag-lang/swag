struct Toto
{
    x, y: s32 = 10
}

impl Toto
{
    #[Swag.implicit]
	func opCast(using self)->s32
	{
		return x + y
	}
}

#test
{
	var x: Toto
	var y: s32 = x
	@assert(y == 20)
}