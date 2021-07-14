#[Swag.inline]
func toto()->s32 => 666

//#[Swag.PrintBc]
#test
{
	x := toto()
	@assert(x == 666)
}
