#global testerror "Err0495"

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
	toto'(true)()
}