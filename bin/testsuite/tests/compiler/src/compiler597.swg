#global testerror "Err0025"

#[Swag.Overload]
func(op: string) toto()
{
}

#[Swag.Overload]
func(op: s32) toto()
{
}

#test
{
	toto()
}