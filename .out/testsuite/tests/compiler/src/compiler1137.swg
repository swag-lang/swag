#global testerror
var b: s32

#[Swag.Inline]
func toto()
{
	b := 666
}

//#[Swag.PrintBc]
#test
{
	toto()
}
