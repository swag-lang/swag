struct Toto
{
}

impl Toto
{
    #[Swag.Implicit]
	func opCast(using self)->s32
	{
		return 666
	}
}

#test
{
	var x: Toto
	let y: s32 = x
	@assert(y == 666)
}