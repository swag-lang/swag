#[swag.inline]
func toto()->s32 => 666

//#[swag.printbc]
#test
{
	x := toto()
	@assert(x == 666)
}
