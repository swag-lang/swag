


#[Swag.constexpr]
func T()->s32 { return 666; }

func F(x: s32 = T())
{
	@assert(x == 666)
}

//#[Swag.printbc]
#test
{
	F();
}