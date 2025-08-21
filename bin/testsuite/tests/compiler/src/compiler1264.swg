struct Toto
{
    x, y: s32 = 10
}

impl Toto
{
    #[Swag.Implicit]
	func opCast(using me)->s32
	{
		return x + y
	}
}

#test
{
	var x: Toto
	let y: s32 = x
	@assert(y == 20)
}