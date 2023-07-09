#[Swag.Inline]
func toto()->s32 => 666

//#[Swag.PrintBc]
#test
{
	var x = toto()
	@assert(x == 666)
}
