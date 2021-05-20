

#[Swag.constexpr]
func toto()->s32
{
	return 666
}

func toto1(x: s32)
{
}

//#[Swag.printbc]
#test
{
	//@print(toto)
	x := 0
	x = toto()
	@assert(x == 666)
}
