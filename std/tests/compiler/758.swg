

#[swag.constexpr]
func toto()->s32
{
	return 666
}

const titi = toto()

//#[swag.printbc]
#test
{	
	@assert(titi == 666)
}
