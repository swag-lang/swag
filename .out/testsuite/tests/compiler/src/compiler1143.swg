#[Swag.inline]
func toto1()->s32
{
	return 1
}

#[Swag.inline]
func toto()->s32
{
	return toto1() + 2
}

//#[Swag.printbc]
#test
{
	@assert(toto() == 3)
	@assert(toto1() == 1)
}