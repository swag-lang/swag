

#[Swag.ConstExpr]
func toto()->s32
{
	return 666
}

const titi = toto()

//#[Swag.PrintBc]
#test
{
	@assert(titi == 666)
}
