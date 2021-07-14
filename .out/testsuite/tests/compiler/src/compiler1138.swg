#[Swag.inline]
func toto()->s32 => 1

//#[Swag.PrintBc]
#test
{
	r := toto()
}
