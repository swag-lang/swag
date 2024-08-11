#global testerror "Err0497"
#global testerror "Err0767"
#global testerror "Err0503"

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