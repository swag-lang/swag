#[Swag.inline]
func toto()->s32 => 666

//#[Swag.printbc]
#test
{
	x := toto()
	@assert(x == 666)
}
