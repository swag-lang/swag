#global testerror "Err0119"
#global testerror "Err0622"
#global testerror "Err0014"
#global testerror "Err0500"

#[Swag.Inline]
func titi()
{
    toto()
}

#[Swag.Inline]
func toto()
{
    titi()
}

//#[Swag.PrintBc]
#test
{
    toto()
}
