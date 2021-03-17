#[swag.inline]
func toto()->s32 => 1

//#[swag.printbc]
#test
{
	r := toto()
}
