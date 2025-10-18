#global testerror "Err0118"
#global testerror "Err0387"
#global testerror "Err0014"
#global testerror "Err0579"

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
