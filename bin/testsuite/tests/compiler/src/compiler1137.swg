#global testerror "Err0627"
var b: s32

#[Swag.Inline]
func toto()
{
	var b = 666
}

//#[Swag.PrintBc]
#test
{
	toto()
}
