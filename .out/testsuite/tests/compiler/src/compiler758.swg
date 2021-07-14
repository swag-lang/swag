

#[Swag.ConstExpr]
func toto()->s32
{
	return 666
}

const titi = toto()

//#[Swag.printbc]
#test
{
	@assert(titi == 666)
}
