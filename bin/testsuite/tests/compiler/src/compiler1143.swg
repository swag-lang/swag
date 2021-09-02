#[Swag.Inline]
func toto1()->s32
{
	return 1
}

#[Swag.Inline]
func toto()->s32
{
	return toto1() + 2
}

//#[Swag.PrintBc]
#test
{
	@assert(toto() == 3)
	@assert(toto1() == 1)
}