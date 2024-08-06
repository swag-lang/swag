#global testerror "Err0557"
#global testerror "Err0767"
#global testerror "Err0563"

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