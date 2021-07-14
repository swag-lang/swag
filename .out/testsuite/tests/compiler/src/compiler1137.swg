var b: s32

#[Swag.inline]
func toto()
{
	b := 666
}

//#[Swag.PrintBc]
#test
{
	toto()
}
