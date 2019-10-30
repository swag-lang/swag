var g: bool = false

#[swag.inline]
func toto1()->s32
{
	return 1
}

#[swag.inline]
func toto()->s32
{
	return toto1()
}

//#[swag.printbc]
#test
{
	@assert(toto() == 1)
}