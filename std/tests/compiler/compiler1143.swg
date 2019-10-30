#[swag.inline]
func toto1()->s32
{
	return 1
}

#[swag.inline]
func toto()->s32
{
	return toto1() + 2
}

//#[swag.printbc]
#test
{
	@assert(toto() == 3)
	@assert(toto1() == 1)
}