

//#[Swag.PrintBc]
#test
{
	func a(x: s32)->s32
	{
		return x;
	}

	let b = a(3)
	@assert(b == 3)
}