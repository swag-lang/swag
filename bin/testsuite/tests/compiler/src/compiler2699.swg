#global testerror "Err0606"
#global testerror "Err0767"
#global testerror "Err0024"

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